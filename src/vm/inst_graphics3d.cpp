#include "inst_graphics3d.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../game.h"
#include "../mem.h"
#include "../log.h"
#include "../scriptvars.h"

#include "../3d/scene.h"

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
  if (Scene3D::Characters[bufferId].Status == LS_Loading ||
      Scene3D::Backgrounds[bufferId].Status == LS_Loading) {
    ResetInstruction;
    BlockThread;
  } else if (ScrWork[SW_CHA1NO + 30 * bufferId] != modelId) {
    ScrWork[SW_CHA1NO + 30 * bufferId] = modelId;
    ScrWork[SW_CHA1EX + 30 * bufferId] = Get5X13Value(modelId);
    if (Get5X13Value(modelId) == 0) {
      Scene3D::Backgrounds[bufferId].LoadAsync(modelId);
    } else {
      Scene3D::Characters[bufferId].LoadAsync(modelId);
    }
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
  if (Scene3D::Characters[bufferId].Status == LS_Loaded && animationId != 0) {
    Scene3D::Characters[bufferId].SwitchAnimation(animationId, 0.66f);
    int loopId =
        64 * ModelIdToCharId[Scene3D::Characters[bufferId].StaticModel->Id -
                             237] +
        (animationId - 1);
    Scene3D::Characters[bufferId].Animator.LoopStart =
        AnimLoopPoints[loopId].LoopStart / AnimDesignFrameRate;
    if (AnimLoopPoints[loopId].LoopEnd != 65536)
      Scene3D::Characters[bufferId].Animator.LoopEnd =
          AnimLoopPoints[loopId].LoopEnd / AnimDesignFrameRate;
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
VmInstruction(InstPositionObject) {
  StartInstruction;
  PopExpression(
      parentObjId);  // 1 -> mainCamera, 2 -> iruoCamera, 10 -> background,
  PopExpression(objectid);  // 11-17 -> characters, 30-38 -> ???
  PopExpression(valueX);    // x
  PopExpression(valueY);    // y
  PopExpression(valueZ);    // z

  float outX, outY, outZ;
  if (parentObjId && objectid) {
    if (parentObjId == 1) {  // main camera
      outX = 0.0f + 0.0f;
      outY = 12.5f + 0.0f;
      outZ = 23.0f + 0.0f;
    } else if (parentObjId == 2) {  // iruo camera
      outX = 0.0f + 0.0f;
      outY = 12.5f + 0.0f;
      outZ = 23.0f + 0.0f;
    } else {
      if (parentObjId < 10 || parentObjId > 17) {
        if (parentObjId >= 30 && parentObjId <= 38) {
          outX = ScrWork[20 * parentObjId + 4900] / 1000.0;
          outY = (ScrWork[20 * parentObjId + 4901] / 1000.0) + 12.5f;
          outZ = ScrWork[20 * parentObjId + 4902] / 1000.0;
        }
      } else {  // background or character
        outX = ScrWork[20 * parentObjId + 5506] / 1000.0;
        outY = ((ScrWork[30 * parentObjId + 4811] +
                 ScrWork[20 * parentObjId + 5507]) /
                1000.0) +
               12.5f;
        outZ = ScrWork[20 * parentObjId + 5508] / 1000.0;
      }
    }
    if (parentObjId != 1 && parentObjId != 2) {
      // I don't even know
      valueX += 180000;
      while (valueX > 180000) {
        valueX -= 360000;
      }
    }

    float xRad = (valueX / 1000.0f) * M_PI / 180.0f;
    float yRad = (valueY / 1000.0f) * M_PI / 180.0f;
    outX = outX + (sinf(xRad) * (cosf(yRad) * (-(valueZ / 1000.0f))));
    outY = outY + (sinf(yRad) * (-(valueZ / 1000.0f)));
    outZ = outZ + (cosf(xRad) * (cosf(yRad) * (-(valueZ / 1000.0f))));

    if (objectid == 1 || objectid == 2) {
      outY = outY - 12.5f;
      outZ = outZ - 23.0f;
    }
    int outYint = (outY * 1000.0f);
    int outZint = (outZ * 1000.0f);
    int outXint = (outX * 1000.0f);
    if (objectid == 1) {  // main camera
      ScrWork[SW_MAINCAMERAPOSX] = outXint;
      ScrWork[SW_MAINCAMERAPOSY] = outYint;
      ScrWork[SW_MAINCAMERAPOSZ] = outZint;
    } else if (objectid == 2) {  // iruo camera
      ScrWork[SW_IRUOCAMERAPOSX] = outXint;
      ScrWork[SW_IRUOCAMERAPOSY] = outYint;
      ScrWork[SW_IRUOCAMERAPOSZ] = outZint;
    } else if (objectid < 10 || objectid > 17) {
      if (objectid >= 30 && objectid <= 38) {
        ScrWork[4900 + 20 * objectid] = outXint;
        ScrWork[4901 + 20 * objectid] = outYint - (12.5f * 1000.0f);
        ScrWork[4902 + 20 * objectid] = outZint;
      }
    } else {  // background or character
      ScrWork[4800 + 30 * objectid] = outXint;
      ScrWork[4801 + 30 * objectid] =
          (outYint - ScrWork[4811 + 30 * objectid]) - (12.5f * 1000.0f);
      ScrWork[4802 + 30 * objectid] = outZint;
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
      someGlobalFloat1 = ScrWork[5424];
      someGlobalFloat2 = arg3;
      someGlobalFloat3 = (arg3 - ScrWork[5424]) / arg5;
    }
    if (arg1 & 2) {
      someGlobalFloat4 = ScrWork[5423];
      someGlobalFloat5 = arg4;
      someGlobalFloat6 = (arg4 - ScrWork[5423]) / arg5;
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
      ScrWork[5424] = arg5;
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
      ScrWork[5423] = arg5;
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
    case 0: {  // main camera
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction Unk0213(type: %i, arg1: %i, arg2: %i, arg3: %i)\n",
          type, arg1, arg2, arg3);
      ScrWork[arg1] = (int)((0.0f + 0.0f) * 1000.0f);
      ScrWork[arg2] = (int)((12.5f + 0.0f) * 1000.0f);
      ScrWork[arg3] = (int)((23.0f + 0.0f) * 1000.0f);
    } break;
    case 1: {
      PopExpression(charId);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      float x = (int)ScrWork[5706 + 20 * charId] / 1000.0f;
      float y = (((int)ScrWork[5707 + 20 * charId] +
                  (int)ScrWork[SW_CHA1YCENTER + 30 * charId]) /
                 1000.0f) +
                12.5;  // main camera default y
      float z = (int)ScrWork[5708 + 20 * charId] / 1000.0f;
      ScrWork[arg2] = (int)(x * 1000.0f);
      ScrWork[arg3] = (int)(y * 1000.0f);
      ScrWork[arg4] = (int)(z * 1000.0f);
    } break;
    case 2: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      float x = (int)ScrWork[5500 + 20 * arg1] / 1000.0;
      float y = ((int)ScrWork[5501 + 20 * arg1] / 1000.0) +
                12.5;  // main camera default y
      float z = (int)ScrWork[5502 + 20 * arg1] / 1000.0;
      ScrWork[arg2] = (int)(x * 1000.0);
      ScrWork[arg3] = (int)(y * 1000.0);
      ScrWork[arg4] = (int)(z * 1000.0);

    } break;
    case 3: {  // iruo camera
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ScrWork[arg1] = (int)((0.0f + 0.0f) * 1000.0f);
      ScrWork[arg2] = (int)((12.5f + 0.0f) * 1000.0f);
      ScrWork[arg3] = (int)((23.0f + 0.0f) * 1000.0f);
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
      PopExpression(arg4);
      PopExpression(arg5);
      glm::vec3 lookat = LookAtEulerZYX(glm::vec3(arg1, arg2, arg3),
                                        glm::vec3(0.0f, 12.5f, 23.0f));

      ScrWork[arg4] = (int)(lookat.y * 1000.0f);
      ScrWork[arg5] = (int)(lookat.x * 1000.0f);

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