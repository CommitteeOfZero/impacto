#include "inst_graphics3d.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../game.h"
#include "../log.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstCHAload3D) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(unk01);
  PopExpression(modelId);
  ImpLogSlow(
      LL_Warning, LC_VMStub,
      "STUB instruction CHAload3D(bufferId: %i, unk01: %i, modelId: %i)\n",
      bufferId, unk01, modelId);
}
VmInstruction(InstCHArelease3D) {
  StartInstruction;
  PopExpression(bufferId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction CHArelease3D(bufferId: %i)\n", bufferId);
}
VmInstruction(InstUnk0204) {
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
  ImpLogSlow(
      LL_Warning, LC_VMStub,
      "STUB instruction CHAplayAnim3DMaybe(bufferId: %i, animationId: %i, "
      "unk01: %i)\n",
      bufferId, animationId, unk01);
}
VmInstruction(InstCHAUnk02073D) {
  StartInstruction;
  PopExpression(bufferId);
  PopUint8(unk01);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction CHAUnk02073D(bufferId: %i, unk01: %i)\n",
             bufferId, unk01);
}
VmInstruction(InstUnk0208) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  PopExpression(arg4);
  PopExpression(arg5);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction Unk0208(arg1: %i, arg2: %i, arg3: %i, arg4: %i, "
             "arg5: %i)\n",
             arg1, arg2, arg3, arg4, arg5);
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
  if (arg1 & 0x10) {
    PopExpression(arg3);
    PopExpression(arg4);
    ImpLogSlow(LL_Warning, LC_VMStub,
               "STUB instruction Unk0210(arg1: %i, arg2: %i, arg3: "
               "%i, arg4: %i)\n",
               arg1, arg2, arg3, arg4);
  } else {
    ImpLogSlow(LL_Warning, LC_VMStub,
               "STUB instruction Unk0210(arg1: %i, arg2: %i)\n", arg1, arg2);
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
VmInstruction(InstUnk0213) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction Unk0213(type: %i, arg1: %i, arg2: %i, arg3: %i)\n",
          type, arg1, arg2, arg3);
    } break;
    case 1: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Unk0213(type: %i, arg1: %i, arg2: %i, arg3: "
                 "%i, arg4: %i)\n",
                 type, arg1, arg2, arg3, arg4);
    } break;
    case 2: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Unk0213(type: %i, arg1: %i, arg2: %i, arg3: "
                 "%i, arg4: %i)\n",
                 type, arg1, arg2, arg3, arg4);
    } break;
    case 3: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction Unk0213(type: %i, arg1: %i, arg2: %i, arg3: %i)\n",
          type, arg1, arg2, arg3);
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
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Unk0218(type: %i, arg1: %i, arg2: %i, arg3: "
                 "%i, arg4: %i, arg5: %i)\n",
                 type, arg1, arg2, arg3, arg4, arg5);
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
VmInstruction(InstUnk0219) {
  StartInstruction;
  PopExpression(bufferId);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Unk0219(bufferId: %i)\n",
             bufferId);
}
VmInstruction(InstUnk0240) {
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

}  // namespace Vm

}  // namespace Impacto