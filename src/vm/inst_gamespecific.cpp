#include "inst_gamespecific.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../game.h"
#include "../log.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstUnk0041) {}
VmInstruction(InstUnk0052) {}
VmInstruction(InstUnk0053) {}
VmInstruction(InstUnk0054) {}
VmInstruction(InstUnk011F) {}
VmInstruction(InstUnk012D) {}
VmInstruction(InstGeotag) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopLocalLabel(label1);
      PopLocalLabel(label2);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Geotag(type: %i)\n",
                 type);
    } break;
    case 1: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Geotag(type: %i, arg1: %i, arg2: %i)\n",
                 type, arg1, arg2);
    } break;
    case 2: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Geotag(type: %i, arg1: %i, arg2: %i)\n",
                 type, arg1, arg2);
    } break;
    case 3: {
      PopExpression(arg1);
      PopLocalLabel(label1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Geotag(type: %i, arg1: %i)\n", type, arg1);
    } break;
    case 4: {
      PopExpression(arg1);
      PopLocalLabel(label1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Geotag(type: %i, arg1: %i)\n", type, arg1);
    } break;
    case 5: {
      PopExpression(arg1);
      PopLocalLabel(label1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Geotag(type: %i, arg1: %i)\n", type, arg1);
    } break;
    case 6: {
      PopExpression(arg1);
      PopLocalLabel(label1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Geotag(type: %i, arg1: %i)\n", type, arg1);
    } break;
    case 7: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Geotag(type: %i, arg1: %i, arg2: %i)\n",
                 type, arg1, arg2);
    } break;
    case 8: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Geotag(type: %i, arg1: %i, arg2: %i)\n",
                 type, arg1, arg2);
    } break;
    case 9: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Geotag(type: %i, arg1: %i, arg2: %i)\n",
                 type, arg1, arg2);
    } break;
    case 10: {
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Geotag(type: %i)\n",
                 type);
    } break;
    case 11: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Geotag(type: %i, arg1: %i, arg2: %i, arg3: "
                 "%i, arg4: %i, arg5: %i)\n",
                 type, arg1, arg2, arg3, arg4, arg5);
    } break;
    case 12: {
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Geotag(type: %i)\n",
                 type);
    } break;
    case 13: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Geotag(type: %i, arg1: %i)\n", type, arg1);
    } break;
    case 14: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Geotag(type: %i, arg1: %i, arg2: %i)\n",
                 type, arg1, arg2);
    } break;
    case 15: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Geotag(type: %i, arg1: %i, arg2: %i, arg3: "
                 "%i, arg4: %i)\n",
                 type, arg1, arg2, arg3, arg4);
    } break;
    case 20: {
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
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Geotag(type: %i, arg1: %i, arg2: %i, arg3: "
                 "%i, arg4: %i, arg5: %i, arg6: %i, arg7: %i, arg8: %i, arg9: "
                 "%i, arg10: %i, arg11: %i, arg12: %i)\n",
                 type, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9,
                 arg10, arg11, arg12);
    } break;
    case 21: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
    } break;
    case 22: {
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Geotag(type: %i)\n",
                 type);
    } break;
  }
}
VmInstruction(InstUnk1037) {}
VmInstruction(InstMail) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Mail(type: %i)\n",
                 type);
      break;
    case 1:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Mail(type: %i)\n",
                 type);
      break;
    case 2:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Mail(type: %i)\n",
                 type);
      break;
    case 3: {
      PopLocalLabel(label1);
      PopLocalLabel(label2);
      PopLocalLabel(label3);
      PopLocalLabel(label4);
      PopLocalLabel(label5);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Mail(type: %i)\n",
                 type);
    } break;
    case 20:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Mail(type: %i)\n",
                 type);
      break;
    case 21: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Mail(type: %i, arg1: %i, arg2: %i, arg3: "
                 "%i, arg4: %i)\n",
                 type, arg1, arg2, arg3, arg4);
    } break;
    case 22: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Mail(type: %i, arg1: %i, arg2: %i, arg3: "
                 "%i, arg4: %i)\n",
                 type, arg1, arg2, arg3, arg4);
    } break;
    case 40:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Mail(type: %i)\n",
                 type);
      break;
    case 50:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Mail(type: %i)\n",
                 type);
      break;
    case 51:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Mail(type: %i)\n",
                 type);
      break;
    case 60:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Mail(type: %i)\n",
                 type);
      break;
    case 61: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Mail(type: %i, arg1: %i)\n", type, arg1);
    } break;
    case 70:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Mail(type: %i)\n",
                 type);
      break;
    case 71: {
      PopLocalLabel(label1);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Mail(type: %i)\n",
                 type);
    } break;
    case 72: {
      PopLocalLabel(label1);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Mail(type: %i)\n",
                 type);
    } break;
    case 73:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Mail(type: %i)\n",
                 type);
      break;
    case 74:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Mail(type: %i)\n",
                 type);
      break;
  }
}
VmInstruction(InstTwipo) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopLocalLabel(label1);
      PopLocalLabel(label2);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Twipo(type: %i)\n",
                 type);
    } break;
    case 1: {
    } break;
    case 2: {
      PopExpression(arg1);
      PopLocalLabel(label1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Twipo(type: %i, arg1: %i)\n", type, arg1);
    } break;
    case 3: {
      PopExpression(arg1);
      PopLocalLabel(label1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Twipo(type: %i, arg1: %i)\n", type, arg1);
    } break;
    case 4: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Twipo(type: %i, arg1: %i)\n", type, arg1);
    } break;
    case 5: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Twipo(type: %i, arg1: %i)\n", type, arg1);
    } break;
    case 6: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Twipo(type: %i, arg1: %i)\n", type, arg1);
    } break;
    case 7: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Twipo(type: %i, arg1: %i)\n", type, arg1);
    } break;
    case 10: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Twipo(type: %i, arg1: %i)\n", type, arg1);
    } break;
    case 11: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Twipo(type: %i, arg1: %i)\n", type, arg1);
    } break;
  }
}
VmInstruction(InstUnk103A) {}

}  // namespace Vm

}  // namespace Impacto