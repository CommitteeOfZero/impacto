#include "inst_graphics3d.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../game.h"
#include "../mem.h"
#include "../log.h"
#include "../profile/scriptvars.h"

#include "../renderer/3d/scene.h"

#include "interface/scene3d.h"

#include "../profile/scene3d.h"
#include "../profile/vm.h"

namespace Impacto {

namespace Vm {

using namespace Impacto::Profile::ScriptVars;

using namespace Interface;

float someGlobalFloat1 = 0.0f;
float someGlobalFloat2 = 0.0f;
float someGlobalFloat3 = 0.0f;
float someGlobalFloat4 = 0.0f;
float someGlobalFloat5 = 0.0f;
float someGlobalFloat6 = 0.0f;

VmInstruction(InstCHAload3D) {
  StartInstruction;
  PopExpression(bufferId);
  switch (Profile::Vm::GameInstructionSet) {
    default:
      break;
    case InstructionSet::RNE:
      PopExpression(unk01);
      break;
  }
  PopExpression(modelId);
  if (Renderer->Scene->Renderables[bufferId]->Status == LoadStatus::Loading) {
    ResetInstruction;
    BlockThread;
  } else if (ScrWork[SW_MDL1FILENO + 30 * bufferId] != modelId) {
    ScrWork[SW_MDL1FILENO + 30 * bufferId] = modelId;

    auto charId = Profile::Scene3D::ModelsToCharacters.find(modelId);
    if (charId != Profile::Scene3D::ModelsToCharacters.end()) {
      ScrWork[SW_MDL1CHANO + 30 * bufferId] = charId->second;
    } else {
      // background
      ScrWork[SW_MDL1CHANO + 30 * bufferId] = 0;
    }

    Renderer->Scene->Renderables[bufferId]->LoadAsync(modelId);
    ResetInstruction;
    BlockThread;
  }
}
VmInstruction(InstCHArelease3D) {
  StartInstruction;
  PopExpression(bufferId);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction CHArelease3D(bufferId: {:d})\n", bufferId);
}
VmInstruction(InstUnk0204) {  // Not implemented
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk0204(arg1: {:d}, arg2: {:d})\n", arg1, arg2);
}
VmInstruction(InstCHAswap3DMaybe) {
  StartInstruction;
  PopExpression(dstBufferId);
  PopExpression(srcBufferId);
  ImpLogSlow(
      LogLevel::Warning, LogChannel::VMStub,
      "STUB instruction CHAswap3DMaybe(dstBufferId: {:d}, srcBufferId: {:d})\n",
      dstBufferId, srcBufferId);
}
VmInstruction(InstCHAplayAnim3DMaybe) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(animationId);
  switch (Profile::Vm::GameInstructionSet) {
    default:
      break;
    case InstructionSet::Dash: {
      PopUint16(unk01);
    } break;
    case InstructionSet::RNE: {
      PopUint8(unk01);
    } break;
  }
  if (Renderer->Scene->Renderables[bufferId]->Status == LoadStatus::Loaded &&
      animationId != 0) {
    // TODO shouldn't this wait for that renderable to be loaded?
    if (Profile::Vm::GameInstructionSet == +InstructionSet::Dash &&
        Renderer->Scene->Renderables[bufferId]->Animator.IsPlaying &&
        Renderer->Scene->Renderables[bufferId]
            ->Animator.CurrentAnimation->OneShot) {
      ResetInstruction;
      BlockThread;
    } else {
      Renderer->Scene->Renderables[bufferId]->SwitchAnimation(
          (int16_t)animationId, 0.66f);
      BlockThread;
    }
  }
}
VmInstruction(InstCHAUnk02073D) {
  StartInstruction;
  PopExpression(bufferId);
  PopUint8(unk01);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction CHAUnk02073D(bufferId: {:d}, unk01: {:d})\n",
             bufferId, unk01);
}
VmInstruction(InstCHAUnk02073D_Dash) {
  StartInstruction;
  PopUint8(arg1);
  PopExpression(arg2);
  if (arg1 == 2) {
    PopExpression(arg3);
    PopExpression(arg4);
    PopLocalLabel(arg5);
    (void)arg5;
    ImpLogSlow(
        LogLevel::Warning, LogChannel::VMStub,
        "STUB instruction CHAUnk02073D_Dash(arg1: {:d}, arg2: {:d}, arg3: "
        "{:d}, arg4: {:d}, arg5: {:d})\n",
        arg1, arg2, arg3, arg4, arg5);
  } else {
    PopLocalLabel(arg3);
    auto dataIp = arg3;
    uint16_t testNum = ScriptBuffers[thread->ScriptBufferId][dataIp + 1];
    if (Renderer->Scene->Renderables[arg2]->Status == LoadStatus::Loaded &&
        testNum != 0) {
      // TODO shouldn't this wait for that renderable to be loaded?
      if (Renderer->Scene->Renderables[arg2]->Animator.IsPlaying &&
          Renderer->Scene->Renderables[arg2]
              ->Animator.CurrentAnimation->OneShot) {
        ResetInstruction;
        BlockThread;
      } else {
        SetFlag(SF_MDL1DISP + arg2, true);
        Renderer->Scene->Renderables[arg2]->SwitchAnimation(testNum, 0);
      }
    }
    ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
               "STUB instruction CHAUnk02073D_Dash(arg1: {:d}, arg2: {:d})\n",
               arg1, arg2);
  }
}

inline bool ObjectIsRenderable(int objectId) {
  return objectId >= 10 && objectId <= 17;
}
// Note: runtime model, not model file
inline int ObjectIdToRenderableId(int objectId) { return objectId - 10; }

// Objects are positioned in a nonstandard spherical coordinate system in
// relation to a parent
VmInstruction(InstPositionObject) {
  StartInstruction;
  PopExpression(
      parentObjId);  // 1 -> mainCamera, 2 -> iruoCamera, 10 -> background,
  PopExpression(objectId);  // 11-17 -> characters, 30-38 -> ???
  PopExpression(theta_);
  PopExpression(phi_);
  PopExpression(radius_);

  float theta = DegToRad(ScrRealToFloat(theta_));
  float phi = DegToRad(ScrRealToFloat(phi_));
  float radius = ScrRealToFloat(radius_);

  // if(ObjectIsRenderable(parentObjId))
  int parentRenderableId = ObjectIdToRenderableId(parentObjId);
  // if(ObjectIsRenderable(objectId))
  int renderableId = ObjectIdToRenderableId(objectId);

  glm::vec3 pos = glm::vec3(0.0f);

  if (parentObjId && objectId) {
    if (parentObjId == 1 || parentObjId == 2) {  // camera
      pos.z += Profile::Scene3D::DefaultCameraPosition.z;
    } else if (ObjectIsRenderable(parentObjId)) {
      // note, these are different than SW_CHAnPOSa ???
      pos = ScrWorkGetVec3(20 * parentRenderableId + 5706,
                           20 * parentRenderableId + 5707,
                           20 * parentRenderableId + 5708);
      pos.y += ScrWorkGetFloat(30 * parentRenderableId + SW_MDL1CENY);
    } else if (parentObjId >= 30 && parentObjId <= 38) {
      pos = ScrWorkGetVec3(20 * parentObjId + 4900, 20 * parentObjId + 4901,
                           20 * parentObjId + 4902);
    }
    pos.y += Profile::Scene3D::DefaultCameraPosition.y;

    if (parentObjId != 1 && parentObjId != 2) {
      // I don't even know
      theta += std::numbers::pi_v<float>;
    }

    theta = NormalizeRad(theta);
    phi = NormalizeRad(phi);

    glm::vec3 sphericalOffset =
        glm::vec3(sinf(theta) * cosf(phi), sinf(phi), cosf(theta) * cosf(phi));
    pos -= radius * sphericalOffset;

    // undo offsets
    pos.y -= Profile::Scene3D::DefaultCameraPosition.y;
    if (objectId == 1 || objectId == 2) {
      pos.z -= Profile::Scene3D::DefaultCameraPosition.z;
    }
    if (ObjectIsRenderable(objectId)) {
      pos.y -= ScrWorkGetFloat(30 * renderableId + SW_MDL1CENY);
    }

    if (objectId == 1) {  // main camera
      ScrWorkSetVec3(SW_MAINCAMERAPOSX, SW_MAINCAMERAPOSY, SW_MAINCAMERAPOSZ,
                     pos);
    } else if (objectId == 2) {  // iruo camera
      ScrWorkSetVec3(SW_IRUOCAMERAPOSX, SW_IRUOCAMERAPOSY, SW_IRUOCAMERAPOSZ,
                     pos);
    } else if (ObjectIsRenderable(objectId)) {
      ScrWorkSetVec3(SW_MDL1POSX + 30 * renderableId,
                     SW_MDL1POSY + 30 * renderableId,
                     SW_MDL1POSZ + 30 * renderableId, pos);
    } else if (objectId >= 30 && objectId <= 38) {
      ScrWorkSetVec3(4900 + 20 * objectId, 4901 + 20 * objectId,
                     4902 + 20 * objectId, pos);
    }
  }
}
VmInstruction(InstPositionObject_Dash) {
  StartInstruction;
  PopExpression(
      parentObjId);  // 1 -> mainCamera, 2 -> iruoCamera, 10 -> background,
  PopExpression(objectId);  // 11-17 -> characters, 30-38 -> ???
  PopExpression(theta_);
  PopExpression(phi_);
  PopExpression(radius_);

  float theta = DegToRad(ScrRealToFloat(theta_));
  float phi = DegToRad(ScrRealToFloat(phi_));
  float radius = ScrRealToFloat(radius_);

  // if(ObjectIsRenderable(parentObjId))
  // int parentRenderableId = ObjectIdToRenderableId(parentObjId);
  // if(ObjectIsRenderable(objectId))
  int renderableId = ObjectIdToRenderableId(objectId);

  glm::vec3 pos = glm::vec3(0.0f);

  if (parentObjId && objectId) {
    if (parentObjId == 1 || parentObjId == 2) {  // camera
      pos = ScrWorkGetVec3(10 * parentObjId + 5390, 10 * parentObjId + 5391,
                           10 * parentObjId + 5392);
    } else if (ObjectIsRenderable(parentObjId)) {
      // note, these are different than SW_CHAnPOSa ???
      pos = ScrWorkGetVec3(20 * parentObjId + 4900, 20 * parentObjId + 4901,
                           20 * parentObjId + 4902);
    } else if (parentObjId >= 30 && parentObjId <= 38) {
      pos = ScrWorkGetVec3(30 * parentObjId + 4800, 30 * parentObjId + 4801,
                           30 * parentObjId + 4802);
    }

    theta = NormalizeRad(theta);
    phi = NormalizeRad(phi);

    pos.x += radius * sinf(theta);
    pos.y -= radius * sinf(phi);
    pos.z -= radius * ((cosf(theta) + cosf(phi)) * 0.5f);

    if (objectId == 1) {  // main camera
      ScrWorkSetVec3(SW_MAINCAMERAPOSX, SW_MAINCAMERAPOSY, SW_MAINCAMERAPOSZ,
                     pos);
    } else if (objectId == 2) {  // iruo camera
      ScrWorkSetVec3(SW_IRUOCAMERAPOSX, SW_IRUOCAMERAPOSY, SW_IRUOCAMERAPOSZ,
                     pos);
    } else if (ObjectIsRenderable(objectId)) {
      ScrWorkSetVec3(SW_MDL1POSX + 30 * renderableId,
                     SW_MDL1POSY + 30 * renderableId,
                     SW_MDL1POSZ + 30 * renderableId, pos);
    } else if (objectId >= 30 && objectId <= 38) {
      ScrWorkSetVec3(4900 + 20 * objectId, 4901 + 20 * objectId,
                     4902 + 20 * objectId, pos);
    }
  }
}
VmInstruction(InstCHAsetAnim3D) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(animationId);
  ImpLogSlow(
      LogLevel::Warning, LogChannel::VMStub,
      "STUB instruction CHAsetAnim3D(bufferId: {:d}, animationId: {:d})\n",
      bufferId, animationId);
}
VmInstruction(InstUnk0210) {
  StartInstruction;
  PopUint8(arg1);
  PopExpression(arg2);
  int arg3, arg4;
  if (arg1 & 0x10) {
    arg3 = ExpressionEval(thread);
    arg4 = ExpressionEval(thread);
  } else {
    arg3 = 0;
    arg4 = 0;
  }
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk0210(arg1: {:d}, arg2: {:d}, arg3: "
             "{:d}, arg4: {:d})\n",
             arg1, arg2, arg3, arg4);
  if (arg1 & 4) {
    int arg5 = 0;
    if (arg2 <= 29) {
      for (int i = 1; i <= arg2; i++) arg5 += i;
    } else {
      arg5 = 30 * arg2 - 870;
      for (int i = 1; i <= 29; i++) arg5 += i;
    }
    if (arg1 & 1) {
      someGlobalFloat1 = (float)ScrWork[SW_MAINCAMERAROTX];
      someGlobalFloat2 = (float)arg3;
      someGlobalFloat3 = (arg3 - (float)ScrWork[SW_MAINCAMERAROTX]) / arg5;
    }
    if (arg1 & 2) {
      someGlobalFloat4 = (float)ScrWork[SW_MAINCAMERAROTY];
      someGlobalFloat5 = (float)arg4;
      someGlobalFloat6 = (arg4 - (float)ScrWork[SW_MAINCAMERAROTY]) / arg5;
    }
  } else {
    if (arg1 & 1) {
      float arg5;
      if (arg2 <= 29) {
        if (arg2 == 1) {
          arg5 = someGlobalFloat2;
        } else {
          arg5 = someGlobalFloat1 + (someGlobalFloat3 * arg2);
        }
      } else {
        arg5 = someGlobalFloat1 + (someGlobalFloat3 * 30.0f);
      }
      someGlobalFloat2 = arg5;
      ScrWork[SW_MAINCAMERAROTX] = (int)arg5;
    }
    if (arg1 & 2) {
      float arg5;
      if (arg2 <= 29) {
        if (arg2 == 1) {
          arg5 = someGlobalFloat5;
        } else {
          arg5 = someGlobalFloat4 + (someGlobalFloat6 * arg2);
        }
      } else {
        arg5 = someGlobalFloat4 + (someGlobalFloat6 * 30.0f);
      }
      someGlobalFloat5 = arg5;
      ScrWork[SW_MAINCAMERAROTY] = (int)arg5;
    }
  }
}
VmInstruction(InstUnk0211) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  PopExpression(arg4);
  PopExpression(arg5);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk0211(arg1: {:d}, arg2: {:d}, arg3: "
             "{:d}, arg4: {:d}, arg5: {:d})\n",
             arg1, arg2, arg3, arg4, arg5);
}
VmInstruction(InstUnk0212) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk0212(arg1: {:d})\n", arg1);
}
VmInstruction(InstUnk0213) {  // Set Camera position ???
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // main camera default position
    case 3:  // iruo camera default position
    {
      PopExpression(outX);
      PopExpression(outY);
      PopExpression(outZ);
      ScrWorkSetVec3(outX, outY, outZ, Profile::Scene3D::DefaultCameraPosition);
    } break;
    case 1: {
      PopExpression(charId);
      PopExpression(outX);
      PopExpression(outY);
      PopExpression(outZ);
      glm::vec3 pos = ScrWorkGetVec3(5706 + 20 * charId, 5707 + 20 * charId,
                                     5708 + 20 * charId);
      pos.y += ScrWorkGetFloat(SW_MDL1CENY + 30 * charId);
      pos.y += Profile::Scene3D::DefaultCameraPosition.y;
      ScrWorkSetVec3(outX, outY, outZ, pos);
    } break;
    case 2: {
      PopExpression(arg1);
      PopExpression(outX);
      PopExpression(outY);
      PopExpression(outZ);
      glm::vec3 pos =
          ScrWorkGetVec3(5500 + 20 * arg1, 5501 + 20 * arg1, 5502 + 20 * arg1);
      pos.y += Profile::Scene3D::DefaultCameraPosition.y;
      ScrWorkSetVec3(outX, outY, outZ, pos);
    } break;
  }
}
VmInstruction(InstUnk0214) {
  StartInstruction;
  PopUint8(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk0214(arg1: {:d}, arg2: {:d}, arg3: {:d})\n",
             arg1, arg2, arg3);
}
VmInstruction(InstUnk0215) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  PopExpression(arg4);
  PopExpression(arg5);
  PopExpression(arg6);
  PopExpression(arg7);
  PopExpression(arg8);
  PopExpression(arg9);
  PopExpression(arg10);
  PopExpression(arg11);
  PopExpression(arg12);
  PopExpression(arg13);
  PopExpression(arg14);
  PopExpression(arg15);
  PopExpression(arg16);
  PopExpression(arg17);
  ImpLogSlow(
      LogLevel::Warning, LogChannel::VMStub,
      "STUB instruction Unk0215(arg1: {:d}, arg2: {:d}, arg3: "
      "{:d}, arg4: {:d}, arg5: {:d}, arg6: {:d}, arg7: {:d}, arg8: {:d}, "
      "arg9: {:d}, arg10: {:d}, arg11: {:d}, arg12: {:d}, arg13: {:d}, arg14: "
      "{:d}, arg15: {:d}, arg16: {:d}, arg17: {:d})\n",
      arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12,
      arg13, arg14, arg15, arg16, arg17);
}

// Temporary test stuff, needs to be done properly
int movementVarDestY[2];
float movementStartY[2];
float movementEndY[2];
float movementYDelta[2];

int movementVarDestX[2];
float movementStartX[2];
float movementEndX[2];
float movementXDelta[2];

VmInstruction(InstMoveCamera) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopExpression(cameraId);
      PopExpression(movStartY);
      PopExpression(movStartX);
      PopExpression(movEndY);
      PopExpression(movEndX);
      PopExpression(length);
      PopExpression(movDestVarY);
      PopExpression(movDestVarX);
      int totalIterations = 0;
      if (length <= 29) {
        for (int i = 1; i <= length; i++) {
          totalIterations += i;
        }
      } else {
        totalIterations = 30 * length - 870;
        int i = 1;
        do {
          totalIterations += i++;
        } while (i <= 29);
      }

      movementVarDestY[cameraId] = movDestVarY;
      movementStartY[cameraId] = ScrRealToFloat(movStartY);
      movementEndY[cameraId] = ScrRealToFloat(movEndY);
      movementYDelta[cameraId] =
          (movementEndY[cameraId] - movementStartY[cameraId]) / totalIterations;

      movementVarDestX[cameraId] = movDestVarX;
      movementStartX[cameraId] = ScrRealToFloat(movStartX);
      movementEndX[cameraId] = ScrRealToFloat(movEndX);
      movementXDelta[cameraId] =
          (movementEndX[cameraId] - movementStartX[cameraId]) / totalIterations;

    } break;
    case 1: {
      PopExpression(cameraId);
      PopExpression(iterations);
      float moveYCur = 0.0f, moveXCur = 0.0f;
      if (iterations <= 29) {
        moveYCur =
            movementStartY[cameraId] + (movementYDelta[cameraId] * iterations);
        moveXCur =
            movementStartX[cameraId] + (movementXDelta[cameraId] * iterations);
      } else {
        moveYCur =
            movementStartY[cameraId] + (movementYDelta[cameraId] * 30.0f);
        moveXCur =
            movementStartX[cameraId] + (movementXDelta[cameraId] * 30.0f);
      }
      movementStartY[cameraId] = moveYCur;
      movementStartX[cameraId] = moveXCur;
      ScrWork[movementVarDestY[cameraId]] = FloatToScrReal(moveYCur);
      ScrWork[movementVarDestX[cameraId]] = FloatToScrReal(moveXCur);
      // meh (hack... I *love* ScrWork crap)
      thread->Variables[7] = FloatToScrReal(moveYCur);
      thread->Variables[8] = FloatToScrReal(moveXCur);
    } break;
  }
}
VmInstruction(InstUnk0217) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk0217(arg1: {:d})\n", arg1);
}
VmInstruction(InstUnk0218) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:
    case 1: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(outYRot);
      PopExpression(outXRot);
      glm::vec3 lookat =
          LookAtEulerZYX(glm::vec3(ScrRealToFloat(arg1), ScrRealToFloat(arg2),
                                   ScrRealToFloat(arg3)),
                         Profile::Scene3D::DefaultCameraPosition);

      ScrWork[outXRot] = -FloatToScrReal(RadToDeg(NormalizeRad(lookat.x)));
      ScrWork[outYRot] = FloatToScrReal(RadToDeg(NormalizeRad(lookat.y)));
    } break;
    case 2: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      PopExpression(arg6);
      PopExpression(arg7);
      PopExpression(arg8);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Unk0218(type: {:d}, arg1: {:d}, arg2: {:d}, arg3: "
          "{:d}, arg4: {:d}, arg5: {:d}, arg6: {:d}, arg7: {:d}, arg8: {:d})\n",
          type, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    } break;
    case 3: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      PopExpression(arg6);
      PopExpression(arg7);
      PopExpression(arg8);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Unk0218(type: {:d}, arg1: {:d}, arg2: {:d}, arg3: "
          "{:d}, arg4: {:d}, arg5: {:d}, arg6: {:d}, arg7: {:d}, arg8: {:d})\n",
          type, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    } break;
    case 4: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      PopExpression(arg6);
      PopExpression(arg7);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Unk0218(type: {:d}, arg1: {:d}, arg2: {:d}, arg3: "
          "{:d}, arg4: {:d}, arg5: {:d}, arg6: {:d}, arg7: {:d})\n",
          type, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    } break;
    default: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Unk0218(type: {:d}, arg1: {:d}, arg2: {:d})\n",
          type, arg1, arg2);
    } break;
  }
}
VmInstruction(InstUnk0219) {  // Not implemented
  StartInstruction;
  PopExpression(bufferId);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk0219(bufferId: {:d})\n", bufferId);
}
VmInstruction(InstUnk0220_Dash) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ScrWorkAnimations[arg1].MainAnimation.DurationIn = arg3 / 1000.0f;
      ScrWorkAnimations[arg1].MainAnimation.DurationOut =
          ScrWorkAnimations[arg1].MainAnimation.DurationIn;
      ScrWorkAnimations[arg1].AltTarget = arg2 != 2;
    } break;
    case 1: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      ScrWorkAnimationData anim;
      anim.Target = arg2;
      anim.From = arg3;
      anim.To = arg4;
      // if (anim.From == anim.To) {
      //  anim.To = 256;
      //}
      if (ScrWorkAnimations[arg1].AltTarget) anim.To = arg5;
      ScrWorkAnimations[arg1].AnimationData.push_back(anim);
    } break;
    case 2: {
      PopExpression(arg1);
      CurrentScrWorkAnimations.push_back(arg1);
      ScrWorkAnimations[arg1].MainAnimation.StartIn();
    } break;
    case 3: {
      PopExpression(arg1);
      PopUint8(arg2);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Unk0220(type: {:d}, arg1: {:d}, arg2: {:d})\n",
          type, arg1, arg2);
      if (ScrWorkAnimations[arg1].MainAnimation.IsIn() ||
          CurrentScrWorkAnimations.size() == 0) {
        ScrWorkAnimations.erase(arg1);
      } else {
        ResetInstruction;
        BlockThread;
      }
    } break;
    case 4: {
      PopUint8(arg1);
      if (!arg1) {
        PopExpression(arg2);
      }
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Unk0220(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 10:
      break;
  }
}
VmInstruction(InstUnk0240) {  // Debug opcode
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
  }
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk0240(type: {:d})\n", type);
}

}  // namespace Vm

}  // namespace Impacto