#include "inst_gamespecific.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../game.h"
#include "../log.h"
#include "../mem.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstUnk0041) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Unk0041()\n");
}
VmInstruction(InstUnk0052) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Unk0052(arg1: %i)\n",
             arg1);
}
VmInstruction(InstUnk0053) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction Unk0053(arg1: %i, arg2: %i)\n", arg1, arg2);
}
VmInstruction(InstUnk0054) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction Unk0054(arg1: %i, arg2: %i)\n", arg1, arg2);
}
VmInstruction(InstUnk011F) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Unk011F()\n");
}
VmInstruction(InstUnk012D) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Unk012D()\n");
}
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
VmInstruction(InstUnk1037) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction Unk1037(arg1: %i, arg2: %i, arg3: %i)\n", arg1,
             arg2, arg3);
}
VmInstruction(InstPhoneSG) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopUint8(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Unk1037(arg1: %i, arg2: %i)\n", arg1, arg2);
      break;
    }
    case 1: {
      PopUint8(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Unk1037(arg1: %i, arg2: %i)\n", arg1, arg2);
      break;
    }
    case 2: {
      PopUint8(arg1);
      PopExpression(arg2);
      PopUint16(arg3);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Unk1037(arg1: %i, arg2: %i, arg3: %i)\n",
                 arg1, arg2, arg3);
      break;
    }
    case 3: {
      PopUint8(arg1);
      PopExpression(arg2);
      PopUint16(arg3);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Unk1037(arg1: %i, arg2: %i, arg3: %i)\n",
                 arg1, arg2, arg3);
      if (GetFlag(2600)) {
        ScrWork[6404] = 20;
      } else {
        ScrWork[6404] = 0;
      }
      break;
    }
    case 4: {
      PopUint16(arg1);
      PopUint16(arg2);
      PopUint16(arg3);
      PopUint16(arg4);
      PopUint16(arg5);
      PopUint16(arg6);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction CHAmoveSetSeqDirect(arg1: %i, arg2: %i, "
                 "arg3: %i, arg4: %i, arg5: %i, arg6: %i)\n",
                 arg1, arg2, arg3, arg4, arg5, arg6);
      break;
    }
    case 5: {  // PhoneInit
      break;
    }
    case 15: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Unk1037(arg1: %i)\n",
                 arg1);
      break;
    }
    case 18: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Unk1037(arg1: %i, arg2: %i)\n", arg1, arg2);
      break;
    }
  }
}
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
VmInstruction(InstUnk1038Darling) {
  StartInstruction;
  PopUint8(type);
  if (type == 0) PopUint16(arg1);
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
VmInstruction(InstUnk103A) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Unk103A(type: %i, arg1: %i, arg2: %i)\n",
                 type, arg1, arg2);
    } break;
    case 1: {
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Unk103A(type: %i)\n",
                 type);
    } break;
    case 2: {
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Unk103A(type: %i)\n",
                 type);
    } break;
  }
}

}  // namespace Vm

}  // namespace Impacto