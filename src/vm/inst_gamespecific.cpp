#include "inst_gamespecific.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../log.h"
#include "../mem.h"
#include "../profile/scriptvars.h"
#include "../video/videosystem.h"
#include "../profile/vm.h"

#include "../ui/gamespecific.h"

#include "../games/cclcc/delusiontrigger.h"
#include "../games/chlcc/delusiontrigger.h"
#include "../games/cclcc/yesnotrigger.h"
#include "../games/cclcc/mapsystem.h"

namespace Impacto {

namespace Vm {

using namespace Impacto::Profile::ScriptVars;

VmInstruction(InstUnk0041) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk0041()\n");
}
VmInstruction(InstUnk0052) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk0052(arg1: {:d})\n", arg1);
}
VmInstruction(InstUnk0053) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk0053(arg1: {:d}, arg2: {:d})\n", arg1, arg2);
}
VmInstruction(InstUnk0054) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk0054(arg1: {:d}, arg2: {:d})\n", arg1, arg2);
}
VmInstruction(InstAddContents) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction AddContents(type: {:d})\n", type);
    } break;
    case 1: {  // CheckDownloadComp
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction AddContents(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 2: {  // OpenContents
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction AddContents(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 3: {  // CloseContents
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction AddContents(type: {:d})\n", type);
    } break;
  }
}
VmInstruction(InstUnk011F) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk011F()\n");
}
VmInstruction(InstUnk012D) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk012D()\n");
}
VmInstruction(InstUnk1035CHLCC) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction InstUnk1035CHLCC()\n");
}
VmInstruction(InstRINNS) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // RINNSInit
      PopUint16(arg1);
      PopUint8(arg2);
      PopExpression(arg3);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction RINNSInit(arg1: {:d}, arg2: {:d}, "
                 "arg3: {:d})\n",
                 arg1, arg2, arg3);
    } break;
    case 0x81: {  // RINNSAddID
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction RINNSAddID(arg1: {:d}, arg2: {:d})\n", arg1,
                 arg2);
    } break;
    case 0x91: {  // RINNSTimeupLogID
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction RINNSTimeupLogID(arg1: {:d})\n", arg1);
    } break;
  }
}
VmInstruction(InstRINNSMain) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction RINNSMain(type: {:d})\n", type);
    } break;
    case 1: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction RINNSMain(type: {:d})\n", type);
    } break;
    case 2: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction RINNSMain(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
  }
}
VmInstruction(InstChatMO8) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // ChatInit
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction ChatMO8(type: {:d})\n", type);
    } break;
    case 1: {  // ChatAdd
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopUint8(arg5);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction ChatMO8(type: {:d}, arg1: {:d}, arg2: {:d}, "
                 "arg3: {:d}, arg4: {:d}, arg5: {:d})\n",
                 type, arg1, arg2, arg3, arg4, arg5);
    } break;
    case 2: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction ChatMO8(type: {:d})\n", type);
    } break;
    case 3: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction ChatMO8(type: {:d})\n", type);
    } break;
    case 4: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction ChatMO8(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 5: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction ChatMO8(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 6: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction ChatMO8(type: {:d}, arg1: {:d}, arg2: {:d}, "
                 "arg3: {:d}, arg4: {:d}, arg5: {:d})\n",
                 type, arg1, arg2, arg3, arg4, arg5);
    } break;
  }
}
VmInstruction(InstGeotag) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopLocalLabel(label1);
      (void)label1;
      PopLocalLabel(label2);
      (void)label2;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Geotag(type: {:d})\n", type);
    } break;
    case 1: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Geotag(type: {:d}, arg1: {:d}, arg2: {:d})\n", type,
          arg1, arg2);
    } break;
    case 2: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Geotag(type: {:d}, arg1: {:d}, arg2: {:d})\n", type,
          arg1, arg2);
    } break;
    case 3: {
      PopExpression(arg1);
      PopLocalLabel(label1);
      (void)label1;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Geotag(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 4: {
      PopExpression(arg1);
      PopLocalLabel(label1);
      (void)label1;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Geotag(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 5: {
      PopExpression(arg1);
      PopLocalLabel(label1);
      (void)label1;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Geotag(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 6: {
      PopExpression(arg1);
      PopLocalLabel(label1);
      (void)label1;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Geotag(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 7: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Geotag(type: {:d}, arg1: {:d}, arg2: {:d})\n", type,
          arg1, arg2);
    } break;
    case 8: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Geotag(type: {:d}, arg1: {:d}, arg2: {:d})\n", type,
          arg1, arg2);
    } break;
    case 9: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Geotag(type: {:d}, arg1: {:d}, arg2: {:d})\n", type,
          arg1, arg2);
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
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Geotag(type: {:d})\n", type);
    } break;
    case 11: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Geotag(type: {:d}, arg1: {:d}, arg2: {:d}, arg3: "
          "{:d}, arg4: {:d}, arg5: {:d})\n",
          type, arg1, arg2, arg3, arg4, arg5);
    } break;
    case 12: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Geotag(type: {:d})\n", type);
    } break;
    case 13: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Geotag(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 14: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Geotag(type: {:d}, arg1: {:d}, arg2: {:d})\n", type,
          arg1, arg2);
    } break;
    case 15: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Geotag(type: {:d}, arg1: {:d}, arg2: {:d}, arg3: "
          "{:d}, arg4: {:d})\n",
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
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Geotag(type: {:d}, arg1: {:d}, arg2: {:d}, arg3: "
          "{:d}, arg4: {:d}, arg5: {:d}, arg6: {:d}, arg7: {:d}, arg8: {:d}, "
          "arg9: {:d}, arg10: {:d}, arg11: {:d}, arg12: {:d})\n",
          type, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10,
          arg11, arg12);
    } break;
    case 21: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
    } break;
    case 22: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Geotag(type: {:d})\n", type);
    } break;
  }
}
VmInstruction(InstUnk100FMO6) {
  StartInstruction;
  PopUint8(type);
  PopExpression(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  ImpLogSlow(
      LogLevel::Warning, LogChannel::VMStub,
      "STUB instruction Unk100FMO6(type: {:d}, arg1: {:d}, arg2: {:d}, arg3: "
      "{:d})\n",
      type, arg1, arg2, arg3);
}
VmInstruction(InstUnk1010MO6) {
  StartInstruction;
  PopUint8(type);
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(
      LogLevel::Warning, LogChannel::VMStub,
      "STUB instruction Unk100FMO6(type: {:d}, arg1: {:d}, arg2: {:d})\n", type,
      arg1, arg2);
}
VmInstruction(InstUnk1011MO6) {
  StartInstruction;
  PopUint8(type);
  PopUint8(type2);
  if (type2 != 2) {
    PopExpression(arg1);
    PopExpression(arg2);
    PopExpression(arg3);
    if (type2 != 0) {
      PopExpression(arg4);
      PopExpression(arg5);
      PopExpression(arg6);
      PopExpression(arg7);
    }
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
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk1037(arg1: {:d}, arg2: {:d}, arg3: {:d})\n",
             arg1, arg2, arg3);
}
VmInstruction(InstMapSystem) {
  StartInstruction;
  PopUint8(type);
  auto& inst = UI::CCLCC::MapSystem::GetInstance();
  switch (type) {
    case 1:
      inst.MapInit();
      break;
    case 2: {
      PopExpression(arg1);
      PopExpression(arg2);
      inst.MapSetFadein(arg1, arg2);
    } break;
    case 3: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction MapSetGroup(arg1: {:d}, arg2: {:d}, arg3: {:d}, "
          "arg4: {:d})\n",
          arg1, arg2, arg3, arg4);
      inst.MapSetGroup(arg1, arg2, arg3, arg4);
    } break;
    case 4: {
      PopExpression(arg1);
      PopExpression(arg2);
      inst.MapSetFadeout(arg1, arg2);
    } break;
    case 5: {
      PopExpression(arg1);
      PopExpression(arg2);
      inst.MapSetDisp(arg1, arg2);
    } break;
    case 6: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MapSetHide(arg1: {:d}, arg2: {:d})\n", arg1,
                 arg2);
      inst.MapSetHide(arg1, arg2);
    } break;
    case 7:
      if (!inst.MapFadeEndChk_Wait()) {
        ResetInstruction;
        BlockThread;
      }
      break;
    case 8: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MapMoveAnimeInit(arg1: {:d}, arg2: {:d}, "
                 "arg3: {:d})\n",
                 arg1, arg2, arg3);
      inst.MapMoveAnimeInit(arg1, arg2, arg3);
    } break;
    case 9:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MapMoveAnimeMain\n");
      if (!inst.MapMoveAnimeMain()) {
        ResetInstruction;
        BlockThread;
      }
      break;
    case 0xA: {
      PopExpression(arg1);
      PopExpression(arg2);
      inst.MapGetPos(arg1, arg2, ScrWork[6365], ScrWork[6366]);

    } break;
    case 0xB: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      inst.MapSetPool(arg2 + arg1 * 10, arg3, arg4);
    } break;
    case 0xC: {
      PopExpression(arg1);
      inst.MapResetPoolAll(arg1);
    } break;
    case 0xD:
      if (!inst.MapPoolFadeEndChk_Wait()) {
        ResetInstruction;
        BlockThread;
      }
      break;
    case 0xE: {
      PopExpression(arg1);
      inst.MapPoolShuffle(arg1);
    } break;
    case 0xF: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MapPoolSetDisp(arg1: {:d}, arg2: {:d})\n",
                 arg1, arg2);
      inst.MapPoolSetDisp(arg1, arg2);
    } break;
    case 0x10: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MapPoolSetHide(arg1: {:d}, arg2: {:d})\n",
                 arg1, arg2);
      inst.MapPoolSetHide(arg1, arg2);
    } break;
    case 0x11: {
      PopExpression(arg1);
      PopExpression(arg2);
      inst.MapPoolSetFadein(arg1, arg2);
    } break;
    case 0x12: {
      PopExpression(arg1);
      PopExpression(arg2);
      inst.MapPoolSetFadeout(arg1, arg2);
    } break;
    case 0x13: {
      PopExpression(arg1);
      if (!inst.MapPlayerPhotoSelect(arg1)) {
        ResetInstruction;
        BlockThread;
      }
    } break;
    case 0x14: {
      PopExpression(arg1);
      PopExpression(arg2);
      inst.MapResetPool(arg1 * 10 + arg2);
    } break;
    case 0x15: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MapSetGroupEx(arg1: {:d}, arg2: {:d}, arg3: "
                 "{:d})\n",
                 arg1, arg2, arg3);
      inst.MapSetGroupEx(arg1, arg2, arg3);
    } break;
    case 0x16: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction MapZoomInit(arg1: {:d}, arg2: {:d}, arg3: {:d})\n",
          arg1, arg2, arg3);
      if (arg1 != ScrWork[6363] || arg2 != ScrWork[6364] ||
          arg3 != ScrWork[6362]) {
        inst.MapZoomInit(arg1, arg2, arg3);
      } else {
        thread->IpOffset += 3;
      }
    } break;
    case 0x17:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MapZoomMain\n");
      if (!inst.MapZoomMain()) {
        ResetInstruction;
        BlockThread;
      }
      break;
    case 0x18: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction MapZoomInit2(arg1: {:d}, arg2: {:d}, arg3: {:d})\n",
          arg1, arg2, arg3);
      if (arg1 != ScrWork[6363] || arg2 != ScrWork[6364] ||
          arg3 != ScrWork[6362]) {
        inst.MapZoomInit2(arg1, arg2);
      } else {
        thread->IpOffset += 3;
      }
    } break;
    case 0x19:
      if (!inst.MapZoomMain3()) {
        ResetInstruction;
        BlockThread;
      }
      break;
    case 0x1A: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      if (arg1 != ScrWork[6363] || arg2 != ScrWork[6364] ||
          arg3 != ScrWork[6362]) {
        if (!inst.MapZoomInit3(arg1, arg2, arg3)) {
          thread->IpOffset += 3;
        }
      } else {
        thread->IpOffset += 3;
      }
    } break;
    case 0x1B: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      if (arg1 != ScrWork[6363] || arg2 != ScrWork[6364] ||
          arg3 != ScrWork[6362]) {
        if (!inst.MapMoveAnimeInit2(arg1, arg2, arg3)) {
          thread->IpOffset += 3;
        }
      } else {
        thread->IpOffset += 3;
      }
    } break;
    case 0x1C:
      if (!inst.MapMoveAnimeMain2()) {
        ResetInstruction;
        BlockThread;
      }
      break;
    case 0x1E:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MapPlayerPotalSelectInit\n");
      inst.MapPlayerPotalSelectInit();
      break;
    case 0x1F:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MapPlayerPotalSelect\n");
      if (!inst.MapPlayerPotalSelect()) {
        ResetInstruction;
        BlockThread;
      }
      break;
    case 0x28:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MapSystem_28\n");
      inst.MapSystem_28();
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
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Unk1037(arg1: {:d}, arg2: {:d})\n", arg1,
                 arg2);
      break;
    }
    case 1: {
      PopUint8(arg1);
      PopExpression(arg2);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Unk1037(arg1: {:d}, arg2: {:d})\n", arg1,
                 arg2);
      break;
    }
    case 2: {
      PopUint8(arg1);
      PopExpression(arg2);
      PopUint16(arg3);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Unk1037(arg1: {:d}, arg2: {:d}, arg3: {:d})\n",
          arg1, arg2, arg3);
      break;
    }
    case 3: {
      PopUint8(arg1);
      PopExpression(arg2);
      PopUint16(arg3);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Unk1037(arg1: {:d}, arg2: {:d}, arg3: {:d})\n",
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
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction CHAmoveSetSeqDirect(arg1: {:d}, arg2: {:d}, "
                 "arg3: {:d}, arg4: {:d}, arg5: {:d}, arg6: {:d})\n",
                 arg1, arg2, arg3, arg4, arg5, arg6);
      break;
    }
    case 5: {  // PhoneInit
      break;
    }
    case 15: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Unk1037(arg1: {:d})\n", arg1);
      break;
    }
    case 18: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Unk1037(arg1: {:d}, arg2: {:d})\n", arg1,
                 arg2);
      break;
    }
  }
}
VmInstruction(InstMail) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Mail(type: {:d})\n", type);
      break;
    case 1:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Mail(type: {:d})\n", type);
      break;
    case 2:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Mail(type: {:d})\n", type);
      break;
    case 3: {
      PopLocalLabel(label1);
      (void)label1;
      PopLocalLabel(label2);
      (void)label2;
      PopLocalLabel(label3);
      (void)label3;
      PopLocalLabel(label4);
      (void)label4;
      PopLocalLabel(label5);
      (void)label5;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Mail(type: {:d})\n", type);
    } break;
    case 20:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Mail(type: {:d})\n", type);
      break;
    case 21: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Mail(type: {:d}, arg1: {:d}, arg2: {:d}, arg3: "
          "{:d}, arg4: {:d})\n",
          type, arg1, arg2, arg3, arg4);
    } break;
    case 22: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Mail(type: {:d}, arg1: {:d}, arg2: {:d}, arg3: "
          "{:d}, arg4: {:d})\n",
          type, arg1, arg2, arg3, arg4);
    } break;
    case 40:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Mail(type: {:d})\n", type);
      break;
    case 50:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Mail(type: {:d})\n", type);
      break;
    case 51:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Mail(type: {:d})\n", type);
      break;
    case 60:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Mail(type: {:d})\n", type);
      break;
    case 61: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Mail(type: {:d}, arg1: {:d})\n", type, arg1);
    } break;
    case 70:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Mail(type: {:d})\n", type);
      break;
    case 71: {
      PopLocalLabel(label1);
      (void)label1;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Mail(type: {:d})\n", type);
    } break;
    case 72: {
      PopLocalLabel(label1);
      (void)label1;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Mail(type: {:d})\n", type);
    } break;
    case 73:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Mail(type: {:d})\n", type);
      break;
    case 74:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Mail(type: {:d})\n", type);
      break;
  }
}
VmInstruction(InstUnk1038Darling) {
  StartInstruction;
  PopUint8(type);
  if (type == 0) {
    PopUint16(arg1);
    (void)arg1;
  }
}
VmInstruction(InstUnk1038MO7) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Unk1038MO7(arg1: {:d}, arg2: {:d})\n", arg1,
             arg2);
}
VmInstruction(InstTwipo) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopLocalLabel(label1);
      (void)label1;
      PopLocalLabel(label2);
      (void)label2;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d})\n", type);
    } break;
    case 1: {
    } break;
    case 2: {
      PopExpression(arg1);
      PopLocalLabel(label1);
      (void)label1;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 3: {
      PopExpression(arg1);
      PopLocalLabel(label1);
      (void)label1;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 4: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 5: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 6: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 7: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 10: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 11: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
  }
}
VmInstruction(InstTwipo_Dash) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopLocalLabel(label1);
      (void)label1;
      PopLocalLabel(label2);
      (void)label2;
      PopLocalLabel(label3);
      (void)label3;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d})\n", type);
    } break;
    case 1:
    case 11:
    case 12:
      break;
    case 2: {
      PopExpression(arg1);
      PopLocalLabel(label1);
      (void)label1;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 3: {
      PopExpression(arg1);
      PopLocalLabel(label1);
      (void)label1;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 4: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 5: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 6: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 7: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 10: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Twipo(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
  }
}
VmInstruction(InstDelusionTriggerCHLCC) {
  StartInstruction;
  PopUint8(type);
  auto& inst = UI::CHLCC::DelusionTrigger::GetInstance();
  switch (type) {
    case 0: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction DelusionTriggerCHLCC(type: {:d})\n", type);
      inst.Hide();
    } break;
    case 1: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction DelusionTriggerCHLCC(type: {:d})\n", type);
      inst.Show();
    } break;
    case 2: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction DelusionTriggerCHLCC(type: {:d})\n", type);
      inst.Show();
      BlockThread;
    } break;
    case 3: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction DelusionTriggerCHLCC(type: {:d})\n", type);
      inst.Show();
    } break;
    case 4: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction DelusionTriggerCHLCC(type: {:d})\n", type);
    } break;
    case 5: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction DelusionTriggerCHLCC(type: {:d})\n", type);
      inst.Show();
    } break;
    case 6: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction DelusionTriggerCHLCC(type: {:d})\n", type);
    } break;
    case 7: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction DelusionTriggerCHLCC(type: {:d})\n", type);
      inst.Hide();
      BlockThread;
    } break;
  }
}

VmInstruction(InstYesNoTriggerCCLCC) {
  StartInstruction;
  PopUint8(type);
  using UI::CCLCC::YesNoTrigger;
  using YesNoState = UI::CCLCC::YesNoTrigger::YesNoState;
  switch (type) {
    case 0: {
      PopExpression(arg1);  // 0, 1, 2, 3
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Unk103A(type: {:d}, arg1: {:d}, arg2: {:d}, "
                 "arg3: {:d})\n",
                 type, arg1, arg2, arg3);
      YesNoTrigger::GetInstance().Start(arg1, arg2, arg3);
    } break;
    case 1: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction YesNoTriggerCCLCC(type: {:d})\n", type);
      // Load images instruction
    } break;
    case 2: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction YesNoTriggerCCLCC(type: {:d})\n", type);
      YesNoTrigger::GetInstance().Show();
      BlockThread;
    } break;
    case 3: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction YesNoTriggerCCLCC(type: {:d})\n", type);
      YesNoTrigger::GetInstance().Hide();
    } break;
    case 4: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction YesNoTriggerCCLCC(type: {:d})\n", type);
      if (YesNoTrigger::GetInstance().State != YesNoState::MainInput) {
        ResetInstruction;
        BlockThread;
      }
    } break;
    case 5: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction YesNoTriggerCCLCC(type: {:d})\n", type);
      YesNoTrigger::GetInstance().AllowInput = true;
    } break;
    case 6: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction YesNoTriggerCCLCC(type: {:d})\n", type);
      if (YesNoTrigger::GetInstance().State == YesNoState::MainInput) {
        ResetInstruction;
        BlockThread;
      }
    } break;
    case 7: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction YesNoTriggerCCLCC(type: {:d})\n", type);
      YesNoTrigger::GetInstance().GoToNextQuestion = true;
    } break;
    case 8: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction YesNoTriggerCCLCC(type: {:d})\n", type);
      PopLocalLabel(branchAddress);
      if (YesNoTrigger::GetInstance().State == YesNoState::PanToNext ||
          YesNoTrigger::GetInstance().State == YesNoState::ZoomStart) {
        ResetInstruction;
        BlockThread;
      } else if (YesNoTrigger::GetInstance().State != YesNoState::Complete) {
        thread->IpOffset = branchAddress;
        BlockThread;
      }

    } break;
    case 10: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction YesNoTriggerCCLCC(type: {:d})\n", type);
      YesNoTrigger::GetInstance().Reset();

    } break;
    default: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction YesNoTriggerCCLCC(type: {:d})\n", type);
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
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction Unk103A(type: {:d}, arg1: {:d}, arg2: {:d})\n",
          type, arg1, arg2);
    } break;
    case 1: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Unk103A(type: {:d})\n", type);
    } break;
    case 2: {
      SetFlag(2951, 1);  // Always win KillBallad
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Unk103A(type: {:d})\n", type);
    } break;
  }
}
VmInstruction(InstUnk1037Noah) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0x0: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_00\n");
    } break;
    case 0x1: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_01\n");
    } break;
    case 0x2: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_01\n");
    } break;
    case 0x3: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_03");
    } break;
    case 0x4: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_04");
    } break;
    case 0x5: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_05");
    } break;
    case 0x6: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_06");
    } break;
    case 0x7: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_07");
    } break;
    case 0x8: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_08");
    } break;
    case 0x9: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_09");
    } break;
    case 0xA: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_0A");
    } break;
    case 0xB: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_0B");
    } break;
    case 0xC: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_0C");
    } break;
    case 0xD: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_0D");
    } break;
    case 0xE: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      PopExpression(arg6);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_0E");
    } break;
    case 0xF: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_0F");
    } break;
    case 0x10: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_10");
    } break;
    case 0x11: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_13");
    } break;
    case 0x12: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_12");
    } break;
    case 0x13: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_13");
    } break;
    case 0x14: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_14");
    } break;
    case 0x15: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_05");
    } break;
    case 0x16: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_16");
    } break;
    case 0x17: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_07");
    } break;
    case 0x18: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_18");
    } break;
    case 0x19: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_19");
    } break;
    case 0x1A: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_1A");
    } break;
    case 0x1B: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_1B");
    } break;
    case 0x1C: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_1C");
    } break;
    case 0x1E: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_1E");
    } break;
    case 0x1F: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_1F");
    } break;
    case 0x20: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_20");
    } break;
    case 0x21: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_21");
    } break;
    case 0x22: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_22");
    } break;
    case 0x28: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_28");
    } break;
    case 0x29: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_29");
    } break;
    case 0x2A: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_2A");
    } break;
    case 0x2B: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_2B");
    } break;
    case 0x2C: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_2C");
    } break;
    case 0x52: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      PopExpression(arg6);
      PopExpression(arg7);
      PopExpression(arg8);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_53");
    } break;
    case 0x53: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      PopExpression(arg6);
      PopExpression(arg7);
      PopExpression(arg8);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_53");
    } break;
    case 0x78: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_78");
    } break;
    case 0x81: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_81");
    } break;
    case 0x8C: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_8C");
    } break;
    case 0x8D: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_8D");
    } break;
    case 0x8E: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_8E");
    } break;
    case 0x8F: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_8F");
    } break;
    case 0x90: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub, "1037_90");
    } break;
  }
}
VmInstruction(InstMtrg) {
  StartInstruction;
  PopUint8(type);

  auto& inst = UI::CCLCC::DelusionTrigger::GetInstance();
  switch (type) {
    case 0: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MtrgStart(type: {:d})\n", type);
      if (inst.Show(arg1, arg2, arg3)) {
        return;
      }
    } break;
    case 1: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MtrgEnd_Wait(type: {:d})\n", type);
      inst.Hide();
      if (ScrWork[SW_DELUSION_BG_COUNTER] != 0) {
        ResetInstruction;
      }
      BlockThread;
      return;
    } break;
    case 2: {
      if (!GetFlag(SF_MOVIEPLAY)) {
        Impacto::Video::Players[0]->Stop();
        SetFlag(2486, 0);
        ScrWork[6333] = 0xffff;
        ScrWork[6336] = 0xffff;
        ScrWork[6344] = 0;
        return;
      }
      if (GetFlag(SF_MESALLSKIP)) {
        Impacto::Video::Players[0]->Stop();
        if ((ScrWork[6333] != 0xffff) && (ScrWork[6336] != 0xffff)) {
          return;
        }
        SetFlag(2486, 0);
        ScrWork[6333] = 0xffff;
        ScrWork[6336] = 0xffff;
        ScrWork[6344] = 0;
        return;
      }
    } break;
    case 3: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MtrgStop_Wait(type: {:d})\n", type);
      if (inst.CheckTransitionAnimationComplete()) {
        SetFlag(SF_DELUSIONSELECTED, 1);
        return;
      }
    } break;
    case 4: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MtrgStart_Wait(type: {:d})\n", type);
      if (inst.CheckStartTransitionComplete()) {
        return;
      }
    } break;
    case 5: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MtrgSetEvent(type: {:d})\n", type);
      return;
    };
    default:
      return;
  }
  ResetInstruction;
  BlockThread;
}

}  // namespace Vm

}  // namespace Impacto