#include "inst_graphics3d.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../game.h"
#include "../mem.h"
#include "../log.h"
#include "../scriptvars.h"

#include "../3d/scene.h"

#include "../profile/scene3d.h"

namespace Impacto {

namespace Vm {

int Get5X13Value(int modelId);
float someGlobalFloat1 = 0.0f;
float someGlobalFloat2 = 0.0f;
float someGlobalFloat3 = 0.0f;
float someGlobalFloat4 = 0.0f;
float someGlobalFloat5 = 0.0f;
float someGlobalFloat6 = 0.0f;

VmInstruction(InstCHAload3D) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(unk01);
  PopExpression(modelId);
  if (Scene3D::Renderables[bufferId].Status == LS_Loading) {
    ResetInstruction;
    BlockThread;
  } else if (ScrWork[SW_CHA1NO + 30 * bufferId] != modelId) {
    ScrWork[SW_CHA1NO + 30 * bufferId] = modelId;
    ScrWork[SW_CHA1EX + 30 * bufferId] = Get5X13Value(modelId);
    Scene3D::Renderables[bufferId].LoadAsync(modelId);
    ResetInstruction;
    BlockThread;
  }
}
VmInstruction(InstCHArelease3D) {
  StartInstruction;
  PopExpression(bufferId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction CHArelease3D(bufferId: %i)\n", bufferId);
}
VmInstruction(InstUnk0204) {  // Not implemented
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction Unk0204(arg1: %i, arg2: %i)\n", arg1, arg2);
}
VmInstruction(InstCHAswap3DMaybe) {
  StartInstruction;
  PopExpression(dstBufferId);
  PopExpression(srcBufferId);
  ImpLogSlow(
      LL_Warning, LC_VMStub,
      "STUB instruction CHAswap3DMaybe(dstBufferId: %i, srcBufferId: %i)\n",
      dstBufferId, srcBufferId);
}
VmInstruction(InstCHAplayAnim3DMaybe) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(animationId);
  PopUint8(unk01);
  if (Scene3D::Renderables[bufferId].Status == LS_Loaded && animationId != 0) {
    Scene3D::Renderables[bufferId].SwitchAnimation(animationId, 0.66f);
    int loopId =
        64 * ModelIdToCharId[Scene3D::Renderables[bufferId].StaticModel->Id -
                             237] +
        (animationId - 1);
    Scene3D::Renderables[bufferId].Animator.LoopStart =
        AnimLoopPoints[loopId].LoopStart /
        Profile::Scene3D::AnimationDesignFrameRate;
    if (AnimLoopPoints[loopId].LoopEnd == 65536) {
      Scene3D::Renderables[bufferId].Animator.OneShot = true;
    } else if (AnimLoopPoints[loopId].LoopEnd != 0) {
      Scene3D::Renderables[bufferId].Animator.LoopEnd =
          AnimLoopPoints[loopId].LoopEnd /
          Profile::Scene3D::AnimationDesignFrameRate;
    }
  }
}
VmInstruction(InstCHAUnk02073D) {
  StartInstruction;
  PopExpression(bufferId);
  PopUint8(unk01);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction CHAUnk02073D(bufferId: %i, unk01: %i)\n",
             bufferId, unk01);
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
      pos.y += ScrWorkGetFloat(30 * parentRenderableId + SW_CHA1YCENTER);
    } else if (parentObjId >= 30 && parentObjId <= 38) {
      pos = ScrWorkGetVec3(20 * parentObjId + 4900, 20 * parentObjId + 4901,
                           20 * parentObjId + 4902);
    }
    pos.y += Profile::Scene3D::DefaultCameraPosition.y;

    if (parentObjId != 1 && parentObjId != 2) {
      // I don't even know
      theta += M_PI;
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
      pos.y -= ScrWorkGetFloat(30 * renderableId + SW_CHA1YCENTER);
    }

    if (objectId == 1) {  // main camera
      ScrWorkSetVec3(SW_MAINCAMERAPOSX, SW_MAINCAMERAPOSY, SW_MAINCAMERAPOSZ,
                     pos);
    } else if (objectId == 2) {  // iruo camera
      ScrWorkSetVec3(SW_IRUOCAMERAPOSX, SW_IRUOCAMERAPOSY, SW_IRUOCAMERAPOSZ,
                     pos);
    } else if (ObjectIsRenderable(objectId)) {
      ScrWorkSetVec3(SW_CHA1POSX + 30 * renderableId,
                     SW_CHA1POSY + 30 * renderableId,
                     SW_CHA1POSZ + 30 * renderableId, pos);
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
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction CHAsetAnim3D(bufferId: %i, animationId: %i)\n",
             bufferId, animationId);
}
VmInstruction(InstUnk0210) {
  StartInstruction;
  PopUint8(arg1);
  PopExpression(arg2);
  int arg3, arg4;
  if (arg1 & 0x10) {
    ExpressionEval(thread, &arg3);
    ExpressionEval(thread, &arg4);
  } else {
    arg3 = 0;
    arg4 = 0;
  }
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction Unk0210(arg1: %i, arg2: %i, arg3: "
             "%i, arg4: %i)\n",
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
      someGlobalFloat1 = ScrWork[SW_MAINCAMERAROTX];
      someGlobalFloat2 = arg3;
      someGlobalFloat3 = (arg3 - ScrWork[SW_MAINCAMERAROTX]) / arg5;
    }
    if (arg1 & 2) {
      someGlobalFloat4 = ScrWork[SW_MAINCAMERAROTY];
      someGlobalFloat5 = arg4;
      someGlobalFloat6 = (arg4 - ScrWork[SW_MAINCAMERAROTY]) / arg5;
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
      ScrWork[SW_MAINCAMERAROTX] = arg5;
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
      ScrWork[SW_MAINCAMERAROTY] = arg5;
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
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction Unk0211(arg1: %i, arg2: %i, arg3: "
             "%i, arg4: %i, arg5: %i)\n",
             arg1, arg2, arg3, arg4, arg5);
}
VmInstruction(InstUnk0212) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Unk0212(arg1: %i)\n",
             arg1);
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
      pos.y += ScrWorkGetFloat(SW_CHA1YCENTER + 30 * charId);
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
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction Unk0214(arg1: %i, arg2: %i, arg3: %i)\n", arg1,
             arg2, arg3);
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
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction Unk0215(type: %i, arg1: %i, arg2: %i, arg3: "
             "%i, arg4: %i, arg5: %i, arg6: %i, arg7: %i, arg8: %i, arg9: %i, "
             "arg10: %i, arg11: %i, arg12: %i, arg13: %i, arg14: %i, arg15: "
             "%i, arg16: %i, arg17: %i)\n",
             arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11,
             arg12, arg13, arg14, arg15, arg16, arg17);
}
VmInstruction(InstUnk0216) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      PopExpression(arg6);
      PopExpression(arg7);
      PopExpression(arg8);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Unk0216(type: %i, arg1: %i, arg2: %i, arg3: "
                 "%i, arg4: %i, arg5: %i, arg6: %i, arg7: %i, arg8: %i)\n",
                 type, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
      ScrWork[arg7] = arg5;
      ScrWork[arg8] = arg6;
    } break;
    case 1: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Unk0216(type: %i, arg1: %i, arg2: %i)\n",
                 type, arg1, arg2);
    } break;
  }
}
VmInstruction(InstUnk0217) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Unk0217(arg1: %i)\n",
             arg1);
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
      PopExpression(outXRot);
      PopExpression(outYRot);
      glm::vec3 lookat =
          LookAtEulerZYX(glm::vec3(ScrRealToFloat(arg1), ScrRealToFloat(arg2),
                                   ScrRealToFloat(arg3)),
                         Profile::Scene3D::DefaultCameraPosition);

      ScrWork[outXRot] = FloatToScrReal(RadToDeg(NormalizeRad(lookat.x)));
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
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Unk0218(type: %i, arg1: %i, arg2: %i, arg3: "
                 "%i, arg4: %i, arg5: %i, arg6: %i, arg7: %i, arg8: %i)\n",
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
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Unk0218(type: %i, arg1: %i, arg2: %i, arg3: "
                 "%i, arg4: %i, arg5: %i, arg6: %i, arg7: %i, arg8: %i)\n",
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
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Unk0218(type: %i, arg1: %i, arg2: %i, arg3: "
                 "%i, arg4: %i, arg5: %i, arg6: %i, arg7: %i)\n",
                 type, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    } break;
    default: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Unk0218(type: %i, arg1: %i, arg2: %i)\n",
                 type, arg1, arg2);
    } break;
  }
}
VmInstruction(InstUnk0219) {  // Not implemented
  StartInstruction;
  PopExpression(bufferId);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Unk0219(bufferId: %i)\n",
             bufferId);
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
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Unk0240(type: %i)\n",
             type);
}

int const Helper5X13Arr[]{
    0x00000000, 0x000000EC, 0x00000000, 0x000000EC, 0x000000ED, 0x000000EE,
    0x000000EF, 0x000000F5, 0x000000F6, 0x000000F9, 0x000000FA, 0x000000FD,
    0x000000FE, 0x000000FF, 0x00000100, 0x00000105, 0x00000106, 0x00000109,
    0x0000010A, 0x0000010B, 0x0000010C, 0x0000010D, 0x0000010E, 0x00000110,
    0x00000111, 0x00000111, 0x00000112, 0x00000112, 0x00000113, 0x00000113,
    0x00000114, 0x00000114, 0x00000115, 0x00000115, 0x00000116, 0x00000116,
    0x00000117, 0x0000011A, 0x00000000, 0x00000000};

int Get5X13Value(int modelId) {
  int pos = 0;
  int res = 0;
  while (modelId < Helper5X13Arr[pos] || modelId > Helper5X13Arr[pos + 1]) {
    ++res;
    pos += 2;
    if (res >= 19) return 255;
  }
  if (res) --res;
  return res;
}

int const ModelIdToCharId[46]{
    0, 0, 1, 1, 1,   1,   1,   1,   1,   2,   2,    2,    2,    3,   3, 3,
    3, 4, 4, 5, 5,   5,   5,   5,   5,   6,   6,    6,    6,    7,   7, 8,
    8, 9, 9, 9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x10, 0x10, 0x10};

AnimLoopPointPair const AnimLoopPoints[1088]{
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 730},   {60, 270},
    {210, 490}, {85, 335},  {240, 520}, {150, 430}, {250, 530}, {190, 470},
    {170, 450}, {95, 525},  {180, 460}, {210, 490}, {300, 580}, {260, 540},
    {150, 430}, {90, 220},  {0, 65536}, {160, 440}, {130, 410}, {0, 730},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {210, 490}, {170, 450},
    {0, 570},   {0, 730},   {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 775},   {169, 595}, {200, 430}, {143, 463},
    {150, 470}, {205, 525}, {180, 500}, {190, 500}, {180, 500}, {175, 495},
    {180, 500}, {185, 505}, {0, 50},    {165, 485}, {205, 525}, {150, 240},
    {0, 65536}, {175, 395}, {180, 420}, {210, 530}, {0, 730},   {0, 65536},
    {150, 470}, {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 600},   {175, 455}, {290, 570}, {290, 570}, {190, 470}, {400, 700},
    {170, 450}, {225, 505}, {225, 505}, {185, 485}, {490, 790}, {0, 600},
    {0, 600},   {180, 460}, {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 690},   {265, 585},
    {210, 530}, {290, 630}, {280, 360}, {220, 540}, {120, 440}, {350, 670},
    {220, 540}, {260, 580}, {400, 700}, {245, 575}, {205, 525}, {225, 545},
    {315, 635}, {210, 530}, {0, 65536}, {0, 65536}, {0, 320},   {0, 65536},
    {0, 80},    {0, 80},    {0, 120},   {0, 80},    {310, 430}, {210, 330},
    {0, 65536}, {90, 330},  {15, 260},  {90, 210},  {0, 80},    {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {90, 705},  {335, 815}, {240, 720}, {220, 700},
    {220, 700}, {255, 735}, {310, 790}, {265, 745}, {260, 740}, {420, 840},
    {370, 790}, {150, 450}, {240, 720}, {240, 720}, {255, 735}, {0, 65536},
    {0, 65536}, {0, 65536}, {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 65536}, {280, 640}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 450},   {0, 65536},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {45, 260},  {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 65536}, {225, 645}, {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {60, 240},  {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 65536}, {175, 455}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 65536}, {260, 720},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {40, 220},  {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {170, 540}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {60, 240},  {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {170, 540}, {170, 540},
    {170, 540}, {170, 540}, {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {60, 210},  {0, 65536},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {80, 264},  {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536},
    {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 65536}, {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 65536}, {215, 535},
    {265, 585}, {300, 620}, {225, 545}, {180, 500}, {215, 535}, {280, 600},
    {230, 550}, {330, 650}, {260, 580}, {270, 590}, {250, 570}, {0, 65536},
    {0, 65536}, {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},     {0, 0},
    {0, 0},     {0, 0}};

}  // namespace Vm

}  // namespace Impacto