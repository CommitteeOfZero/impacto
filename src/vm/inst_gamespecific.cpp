#include "inst_gamespecific.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../game.h"
#include "../log.h"
#include "../mem.h"
#include "../profile/scriptvars.h"

namespace Impacto {

namespace Vm {

using namespace Impacto::Profile::ScriptVars;

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
      for (int i = 0; i < 8; i++) {
        if (ScrWork[SW_CHA1EX + 30 * i] == 0)
          SetFlag(SF_MDL1SHDISP + i, 1);
        else {
          if (GetFlag(SF_AR_SETUP_ADD_MDLBUF1 + i))
            SetFlag(SF_MDL1SHDISP + i, 1);
          else
            SetFlag(SF_MDL1SHDISP + i, 0);
        }
      }
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
VmInstruction(InstUnk1037MO7) {
  StartInstruction;
  PopUint8(type);
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
VmInstruction(InstMapSystem) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 1:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapInit");
      break;
    case 2: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub, "MapSetFadein");
    } break;
    case 3: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapSetGroup");
    } break;
    case 4: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapSetFadeout");
    } break;
    case 5: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapSetDisp");
    } break;
    case 6: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapSetHide");
    } break;
    case 7:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapFadeEndChk_Wait");
      break;
    case 8: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapMoveAnimeInit");
    } break;
    case 9:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapMoveAnimeMain");
      break;
    case 0xA: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapGetPos");
    } break;
    case 0xB: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapSetPool");
    } break;
    case 0xC: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapResetPoolAll");
    } break;
    case 0xD:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MapPoolFadeEndChk_Wait");
      break;
    case 0xE: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapPoolShuffle");
    } break;
    case 0xF: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapPoolSetDisp");
    } break;
    case 0x10: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapPoolSetHide");
    } break;
    case 0x11: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapPoolSetFadein");
    } break;
    case 0x12: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapPoolSetFadeout");
    } break;
    case 0x13: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MapPlayerPhotoSelect");
    } break;
    case 0x14: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapResetPool");
    } break;
    case 0x15: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapSetGroupEx");
    } break;
    case 0x16: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapZoomInit");
    } break;
    case 0x17:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapZoomMain");
      break;
    case 0x18: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapZoomInit2");
    } break;
    case 0x19:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapZoomMain3");
      break;
    case 0x1A: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapZoomInit3");
    } break;
    case 0x1B: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapMoveAnimeInit2");
    } break;
    case 0x1C:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapMoveAnimeMain2");
      break;
    case 0x1E:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MapPlayerPotalSelectInit");
      break;
    case 0x1F:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MapPlayerPotalSelect");
      break;
    case 0x28:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MapSystem_28");
      break;
  }
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
      if (GetFlag(SF_Phone_Open)) {
        ScrWork[SW_PHONE_DISP_CT] = 20;
      } else {
        ScrWork[SW_PHONE_DISP_CT] = 0;
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
VmInstruction(InstUnk1038MO7) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction Unk1038MO7(arg1: %i, arg2: %i)\n", arg1, arg2);
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
VmInstruction(InstTwipo_Dash) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopLocalLabel(label1);
      PopLocalLabel(label2);
      PopLocalLabel(label3);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Twipo(type: %i)\n",
                 type);
    } break;
    case 1:
    case 11:
    case 12:
      break;
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
  }
}
VmInstruction(InstDelusionTriggerCHLCC) {
  StartInstruction;
  PopUint8(type);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction DelusionTriggerCHLCC()\n");
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
      SetFlag(2951, 1);  // Always win KillBallad
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Unk103A(type: %i)\n",
                 type);
    } break;
  }
}

}  // namespace Vm

}  // namespace Impacto