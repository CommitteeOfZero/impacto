#include "inst_system.h"

#include "inst_macros.inc"

#include <math.h>
#include <ctime>

#include "expression.h"
#include "../profile/scriptvars.h"
#include "../profile/scriptinput.h"
#include "../mem.h"
#include "../log.h"
#include "../audio/audiosystem.h"
#include "../audio/audiochannel.h"
#include "../background2d.h"
#include "../character2d.h"
#include "../profile/vm.h"
#include "../profile/dialogue.h"
#include "../profile/hud/tipsnotification.h"
#include "../profile/hud/delusiontrigger.h"
#include "../profile/data/tipssystem.h"
#include "../profile/ui/backlogmenu.h"
#include "../hud/saveicondisplay.h"
#include "interface/input.h"
#include "../data/savesystem.h"
#include "../data/achievementsystem.h"
#include "../ui/ui.h"
#include "../voicetable.h"

namespace Impacto {

namespace Vm {

using namespace Impacto::SaveSystem;
using namespace Impacto::TipsSystem;
using namespace Impacto::Profile::ScriptVars;

VmInstruction(InstDummy) {}

VmInstruction(InstEnd) {
  StartInstruction;
  thread->Flags = TF_Destroy;
  BlockThread;
}
VmInstruction(InstCreateThread) {
  StartInstruction;
  PopExpression(groupId);
  PopExpression(scriptBufferId);
  PopFarLabel(labelAdr, scriptBufferId);
  Sc3VmThread* newThread = CreateThread(groupId);
  newThread->GroupId = groupId;
  newThread->ScriptBufferId = scriptBufferId;
  newThread->Ip = labelAdr;
  thread->ScriptParam = newThread->Id;
  newThread->ScriptParam = thread->Id;
  RunThread(newThread);
  BlockCurrentScriptThread = false;
}
VmInstruction(InstKillThread) {
  StartInstruction;
  PopExpression(threadId);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction KillThread(threadId: {:d})\n", threadId);
}
VmInstruction(InstReset) {
  StartInstruction;
  BlockThread;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Reset()\n");
}
VmInstruction(InstScriptLoad) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(scriptId);
  if (Profile::Vm::UseMsbStrings) {
    LoadMsb(bufferId, scriptId);
    if (!Profile::Vm::UseSeparateMsbArchive) scriptId += 1;
  }
  LoadScript(bufferId, scriptId);
}
VmInstruction(InstWait) {
  StartInstruction;
  PopExpression(waitCycles);
  if (thread->WaitCounter <= 0) {
    thread->WaitCounter = waitCycles + 1;
  }
  thread->WaitCounter--;
  if (thread->WaitCounter > 0) {
    ResetInstruction;
    BlockThread;
  }
}
VmInstruction(InstHalt) {
  StartInstruction;
  thread->Flags |= TF_Pause;
  BlockThread;
  ResetInstruction;
}
VmInstruction(InstGetLabelAdr) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction GetLabelAdr(unknown)\n");
}
VmInstruction(InstFlagOnWait) {
  StartInstruction;
  PopUint8(checkVal);
  PopExpression(flagId);
  if (GetFlag(flagId) == (bool)checkVal) {
    ResetInstruction;
    BlockThread;
  }
}
VmInstruction(InstSetFlag) {
  StartInstruction;
  PopExpression(flagId);
  SetFlag(flagId, 1);
}
VmInstruction(InstResetFlag) {
  StartInstruction;
  PopExpression(flagId);
  SetFlag(flagId, 0);
}
VmInstruction(InstCopyFlag) {
  StartInstruction;
  PopExpression(flagIdSrc);
  PopExpression(flagIdDst);
  bool flagValSrc = GetFlag(flagIdSrc);
  SetFlag(flagIdDst, flagValSrc);
}
VmInstruction(InstKeyWait) {
  StartInstruction;
  PopUint8(type);
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction KeyWait(type: {:d}, arg1: {:d}, arg2: {:d})\n",
             type, arg1, arg2);
}
VmInstruction(InstKeyWaitTimer) {
  StartInstruction;
  PopUint8(type);
  PopExpression(timer);
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(
      LogLevel::Warning, LogChannel::VMStub,
      "STUB instruction KeyWaitTimer(type: {:d}, timer: {:d}, arg1: {:d}, "
      "arg2: {:d})\n",
      type, timer, arg1, arg2);
}
VmInstruction(InstMemberWrite) {
  StartInstruction;
  if (noExpressions) {
    PopUint8(index);
    void* thdElement = thread->GetMemberPointer(index);
    uint8_t* immValue = thread->Ip;
    int value = immValue[0] + (immValue[1] << 8) + (immValue[2] << 16) +
                (immValue[3] << 24);
    thread->Ip += 4;
    UnalignedWrite<uint32_t>(thdElement, value);
  } else {
    PopExpression(index);
    void* thdElement = thread->GetMemberPointer(index);
    PopExpression(value);
    UnalignedWrite<uint32_t>(thdElement, value);
  }
}
VmInstruction(InstThreadControl) {
  StartInstruction;
  PopExpression(groupId);
  PopExpression(controlType);
  ControlThreadGroup((ThreadGroupControlType)controlType, groupId);
}
VmInstruction(InstGetSelfPointer) {
  StartInstruction;
  thread->ScriptParam = thread->Id ^ 0x80000000;
}
VmInstruction(InstVsync) {
  StartInstruction;
  BlockThread;
}
VmInstruction(InstTest) {
  StartInstruction;
  PopExpression(testVal);
}
VmInstruction(InstThreadControlStore) {
  StartInstruction;
  PopUint8(type);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction ThreadControlStore(type: {:d})\n", type);
}
VmInstruction(InstPadAct) {
  StartInstruction;
  PopExpression(unused);
  PopExpression(vib1);
  PopExpression(vib2);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction PadAct(unused: {:d}, vib1: {:d}, vib2: {:d})\n",
             unused, vib1, vib2);
}
VmInstruction(InstCopyThreadWork) {
  StartInstruction;
  PopExpression(threadIdSrc);
  PopExpression(threadIdDst);
  PopExpression(beginIndex);
  PopExpression(count);
  Sc3VmThread *srcThread, *dstThread;
  srcThread = threadIdSrc == 0 ? thread : &ThreadPool[threadIdSrc];
  dstThread = threadIdDst == 0 ? thread : &ThreadPool[threadIdDst];

  for (int i = 0; i < count; i++) {
    dstThread->Variables[beginIndex + i] = srcThread->Variables[beginIndex + i];
  }
}

inline void LoadSaveFile() {
  ScrWork[SW_SAVEERRORCODE] = SaveSystem::MountSaveFile();
  if (ScrWork[SW_SAVEERRORCODE] == SaveOK) {
    UpdateTipRecords();
  }
}

VmInstruction(InstSave) {
  StartInstruction;
  PopUint8(type);
  switch (type) {  // TODO: Types 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                   // 16, 20, 21, 72, 30, 31, 32, 33, 34, 35, 41, 50, 51, 66,
                   // 67, 70, 71, 74, 76
    case 0: {
      // TODO, System Save only
    } break;
    case 3:
      break;
    case 4: {
    } break;
    case 32: {
      if (Profile::Vm::GameInstructionSet == +InstructionSet::MO6TW ||
          Profile::Vm::GameInstructionSet == +InstructionSet::CHLCC) {
        LoadSaveFile();
      }
    } break;
    case 40:  // SystemDataCheck
      if (Profile::Vm::GameInstructionSet == +InstructionSet::RNE) {
        PopExpression(unused1);
        PopExpression(unused2);
        PopExpression(unused3);
        PopExpression(unused4);
      }
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Save(type: {:d})\n", type);
      break;
    case 16:
      SaveSystem::FlushWorkingSaveEntry(SaveSystem::SaveType::SaveFull,
                                        ScrWork[SW_SAVEFILENO]);
      SaveSystem::WriteSaveFile();
      break;
    case 50:
      if (Profile::Vm::GameInstructionSet == +InstructionSet::CHLCC) {
        AchievementSystem::MountAchievementFile();
      }
      break;
    case 51:
      break;
    case 60:
      SaveSystem::WriteSaveFile();
      break;
    case 70:
      if (Profile::Vm::GameInstructionSet == +InstructionSet::CC) {
        LoadSaveFile();
      }
    case 30:
      break;
    case 31:
      if (Profile::Vm::GameInstructionSet == +InstructionSet::CC) {
        ScrWork[SW_SAVEERRORCODE] = CreateSaveFile();
      }
      break;
    case 80:
      break;
    case 81: {  // SystemDataCheck
      if (Profile::Vm::GameInstructionSet == +InstructionSet::CC) {
        ScrWork[SW_SAVEERRORCODE] = CheckSaveFile();
      }
    } break;
    default:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Save(type: {:d})\n", type);
      break;
  }
}
VmInstruction(InstSaveIconLoad) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SaveIconLoad(arg1: {:d})\n", arg1);
}
VmInstruction(InstVoiceTableLoadMaybe) {
  StartInstruction;
  PopExpression(fileId);
  if (VoiceTableData.Status == LS_Loading) {
    ResetInstruction;
    BlockThread;
  } else if (VoiceTableData.Status == LS_Unloaded) {
    VoiceTableData.LoadAsync(fileId);
    ResetInstruction;
    BlockThread;
  }
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction VoiceTableLoad(arg1: {:d})\n", fileId);
}
VmInstruction(InstSetPadCustom) {
  StartInstruction;
  Interface::UpdatePADcustomType(Profile::ScriptInput::PADcustomType);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SetPadCustom()\n");
}
VmInstruction(InstMwait) {
  StartInstruction;
  PopExpression(waitCycles);
  PopExpression(unused);
  if (thread->WaitCounter <= 0) {
    thread->WaitCounter = waitCycles + 1;
  }
  thread->WaitCounter--;
  // This wait is ignored if skip mode is enabled
  if (GetFlag(SF_MESALLSKIP)) {
    thread->WaitCounter = 0;
  }
  if (thread->WaitCounter > 0) {
    ResetInstruction;
    BlockThread;
  }
}
VmInstruction(InstTerminate) {
  StartInstruction;

  ImpLog(LogLevel::Info, LogChannel::VM, "VM requested shutdown!\n");
  Window->Shutdown();
  // BlockThread;
  // ResetInstruction;
}
VmInstruction(InstDebugPrint) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LogLevel::Debug, LogChannel::VM,
             "DebugPrint(arg1: {:d}, arg2: {:d})\n", arg1, arg2);
}
VmInstruction(InstSystemMes) {
  StartInstruction;
  PopUint8(mode);
  switch (Profile::Vm::GameInstructionSet) {
    default:
      break;
    case InstructionSet::Dash:
    case InstructionSet::CC:
      PopUint8(unk01);
      break;
  }

  switch (mode) {
    case 0:  // SystemMesInit0
    case 1:  // SystemMesInit1
      UI::SysMesBoxPtr->Init();
      break;
    case 2: {  // SystemMesInit2
      PopExpression(sysMesInit2Arg);
      ScrWork[SW_SYSMESANIMCTF] = 2 * UI::SysMesBoxPtr->MessageCount + 33;
    } break;
    case 3: {  // SystemMesSetMes
      PopUint16(sysMesStrNum);
      UI::SysMesBoxPtr->AddMessage(ScriptGetStrAddress(
          ScriptBuffers[thread->ScriptBufferId], sysMesStrNum));
    } break;
    case 4: {  // SystemMesSetSel
      PopUint16(sysSelStrNum);
      UI::SysMesBoxPtr->AddChoice(ScriptGetStrAddress(
          ScriptBuffers[thread->ScriptBufferId], sysSelStrNum));
    } break;
    case 5:  // SystemMesMain
      if (!UI::SysMesBoxPtr->ChoiceMade &&
          (UI::SysMesBoxPtr->ChoiceCount > 0)) {
        ResetInstruction;
        BlockThread;
      } else if (UI::SysMesBoxPtr->ChoiceCount == 0 &&
                 !(Interface::PADinputButtonWentDown & Interface::PAD1A ||
                   Interface::PADinputMouseWentDown & Interface::PAD1A)) {
        ResetInstruction;
        BlockThread;
      }
      break;
    case 6:  // SystemMesFadeIn
      if (UI::SysMesBoxPtr->State == UI::MenuState::Hidden) {
        UI::SysMesBoxPtr->Show();
        ResetInstruction;
        BlockThread;
      } else if (UI::SysMesBoxPtr->State != UI::MenuState::Shown) {
        ResetInstruction;
        BlockThread;
      }
      break;
    case 7:  // SystemMesFadeOut
      if (UI::SysMesBoxPtr->State == UI::MenuState::Shown) {
        UI::SysMesBoxPtr->Hide();
        ResetInstruction;
        BlockThread;
      } else if (UI::SysMesBoxPtr->State != UI::MenuState::Hidden) {
        ResetInstruction;
        BlockThread;
      }
      break;
    case 8:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SystemMes(mode: {:d})\n", mode);
      break;
    case 20:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SystemMes(mode: {:d})\n", mode);
      break;
    case 0x83: {
      PopMsbString(message);
      UI::SysMesBoxPtr->AddMessage(message);
    } break;
    case 0x84: {  // SystemMesSetSel
      PopMsbString(message);
      UI::SysMesBoxPtr->AddChoice(message);
    } break;
    default:
      ImpLog(LogLevel::Warning, LogChannel::VMStub,
             "Unknown mode for instruction SystemMes(mode: {:d})\n", mode);
      break;
  }
}
VmInstruction(InstGetNowTime) {
  StartInstruction;
  std::time_t t = std::time(0);
  std::tm* now = std::localtime(&t);
  ScrWork[SW_TIMEYEAR] = now->tm_year + 1900;
  ScrWork[SW_TIMEMONTH] = now->tm_mon + 1;
  ScrWork[SW_TIMEDAY] = now->tm_mday;
  ScrWork[SW_TIMEHOUR] = now->tm_hour;
  ScrWork[SW_TIMEMINUTE] = now->tm_min;
  ScrWork[SW_TIMESECOND] = now->tm_sec;
  ScrWork[SW_TIMEWEEK] = now->tm_wday;
}
VmInstruction(InstGetSystemStatus) {
  StartInstruction;
  PopExpression(type);
  switch (type) {
    case 1: {  // SYSSTAT_VOICEPLAY (Note, called voiceplay, but actually
               // !voiceplay...)
      thread->ScriptParam =
          Audio::Channels[Audio::AC_VOICE0]->State != Audio::ACS_Playing;
      break;
    }
    case 5: {  // SYSSTAT_SKIP
      thread->ScriptParam = GetFlag(SF_MESALLSKIP);
    } break;
    case 10: {  // Auto mode bit
      thread->ScriptParam = (MesSkipMode & SkipModeFlags::Auto) >> 2;
    }
    case 11: {
      thread->ScriptParam = 1;
      break;
    }
  }
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction GetSystemStatus(type: {:d})\n", type);
}
VmInstruction(InstReboot) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Reboot()\n");
  BlockThread;
}
VmInstruction(InstReloadScript) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction ReloadScript()\n");
  BlockThread;
  ResetInstruction;
}
VmInstruction(InstReloadScriptMenu) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction ReloadScriptMenu(arg1: {:d})\n", arg1);
}
VmInstruction(InstDebugEditer) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction DebugEditer(arg1: {:d})\n", arg1);
}
VmInstruction(InstPadActEx) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction PadActEx(arg1: {:d}, arg2: {:d}, arg3: "
             "{:d})\n",
             arg1, arg2, arg3);
}
VmInstruction(InstDebugSetup) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction DebugSetup(arg1: {:d})\n", arg1);
}
VmInstruction(InstGlobalSystemMessage) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopExpression(alpha);
      // TODO: set GlobalSystemMessageCur to 255
      ScrWork[SW_SYSMESALPHA] = alpha;
    } break;
    case 1:
      ScrWork[SW_SYSMESANIMCTCUR] += 8;
      break;
    case 2:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction GlobalSystemMessage(type: 2)\n");
      break;
    case 3:
      ScrWork[SW_SYSMESANIMCTCUR] -= 8;
      if (!ScrWork[SW_SYSMESANIMCTCUR]) ScrWork[3370] = 255;
      break;
  }
}
// TODO find the value ranges for atan2
VmInstruction(InstCalc) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // CalcSin
      PopExpression(dest);
      PopExpression(angle);
      ScrWork[dest] = ((int)std::sin(angle / (65536 / (2 * M_PI)))) * 65536;
    } break;
    case 1: {  // CalcCos
      PopExpression(dest);
      PopExpression(angle);
      ScrWork[dest] = ((int)std::cos(angle / (65536 / (2 * M_PI)))) * 65536;
    } break;
    case 2: {  // CalcAtan2
      PopExpression(dest);
      PopExpression(x);
      PopExpression(y);
      ScrWork[dest] = (int)atan2(x, y);
    } break;
    case 3: {  // CalcSinL
      PopExpression(dest);
      PopExpression(base);
      PopExpression(angle);
      PopExpression(offset);
      ScrWork[dest] =
          offset +
          base * ((int)(std::sin(angle / (65536 / (2 * M_PI)))) * 65536);
    } break;
    case 4: {  // CalcCosL
      PopExpression(dest);
      PopExpression(base);
      PopExpression(angle);
      PopExpression(offset);
      ScrWork[dest] =
          offset +
          base * ((int)(std::cos(angle / (65536 / (2 * M_PI)))) * 65536);
    } break;
    case 5: {  // CalcRound
      PopExpression(dest);
      PopExpression(value);
      PopExpression(multiplier);
      PopExpression(divider);
      ScrWork[dest] =
          (int)((((((multiplier * value) * 10.0) / divider) + 5.0) / 10.0));
    } break;
    case 6: {  // CalcAccel
      PopExpression(dest);
      PopExpression(x);
      PopExpression(a);
      PopExpression(b);
      if (b >= 2) {
        ScrWork[dest] = (20 * a * x / b + 5 - 10 * a * a * x / b / b) / 10;
      } else {
        ScrWork[dest] = x;
      }
    } break;
  }
}
VmInstruction(InstMSinit) {
  StartInstruction;
  PopExpression(
      initType);  // TODO: There's only one type in R;NE - initType <= 10
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction MSinit(initType: {:d})\n", initType);
  SaveSystem::SetCheckpointId(0xffff);
  if (initType == 0) {
    if (Profile::Vm::GameInstructionSet == +InstructionSet::MO6TW ||
        Profile::Vm::GameInstructionSet == +InstructionSet::CHLCC) {
      memset(&FlagWork, 0, 500);
      memset(&ScrWork, 0, 24000);
    } else if (Profile::Vm::GameInstructionSet == +InstructionSet::CC) {
      memset(&FlagWork, 0, 1000);
      memset(&ScrWork, 0, 32000);
    }

    ScrWork[SW_SYSMESALPHA] = 255;

    ScrWork[SW_PLATFORM] = Profile::PlatformId;
  }

  if (initType == 0 || initType == 1) {
    ScrWork[SW_GAMESTATE] = 0;
    for (int i = 0; i < DialoguePageCount; i++) {
      DialoguePages[i].Clear();
      DialoguePages[i].FadeAnimation.Progress = 0;
      SetFlag(i + SF_MESWINDOW0OPENFL, 0);
    }
  }

  if (initType == 5) {
    UI::BacklogMenuPtr->Clear();
  }

  if (initType == 10) {
    Profile::TipsNotification::CreateInstance();
    Profile::DelusionTrigger::CreateInstance();
  }

  if (initType == 2) {
    UI::BacklogMenuPtr->Clear();
    memset(&FlagWork, 0, 100);
    memset(&FlagWork[150], 0, 75);
    memset(&FlagWork[300], 0, 100);

    if (Profile::Vm::GameInstructionSet == +InstructionSet::MO6TW ||
        Profile::Vm::GameInstructionSet == +InstructionSet::CHLCC) {
      std::fill(ScrWork.begin(), ScrWork.begin() + 600, 0);
      std::fill(ScrWork.begin() + 1000, ScrWork.begin() + 1700, 0);
      std::fill(ScrWork.begin() + 2300, ScrWork.begin() + 2320, 0);
      std::fill(ScrWork.begin() + 2328, ScrWork.begin() + 3600, 0);
    } else {
      std::fill(ScrWork.begin(), ScrWork.begin() + 1600, 0);
      std::fill(ScrWork.begin() + 2100, ScrWork.begin() + 3300, 0);
      std::fill(ScrWork.begin() + 4347, ScrWork.begin() + 7300, 0);
    }
  }

  for (int i = 0; i < MaxBackgrounds2D; i++) {
    ScrWork[SW_BG1SURF + i] = i;
    ScrWork[SW_BG1ALPHA + Profile::Vm::ScrWorkBgStructSize * i] = 256;
    ScrWork[SW_BG1NO + Profile::Vm::ScrWorkBgStructSize * i] = 0xFFFF;
  }
  for (int i = 0; i < MaxCharacters2D; i++) {
    ScrWork[SW_CHA1SURF + i] = i;
    ScrWork[SW_CHA1ALPHA + Profile::Vm::ScrWorkChaStructSize * i] = 256;
  }

  if (Profile::Dialogue::HasSpeakerPortraits) {
    for (int i = 0; i < MaxSpeakerPortraits; i++) {
      ScrWork[SW_FACE1SURF + i] = i;
    }
  }

  ScrWork[SW_MESWINDOW_COLOR] = 0xFFFFFF;
  ScrWork[SW_BGMREQNO] = 0xFFFF;
  ScrWork[SW_SEREQNO] = 0xFFFF;
  ScrWork[SW_SEREQNO + 1] = 0xFFFF;
  ScrWork[SW_SEREQNO + 2] = 0xFFFF;
  ScrWork[SW_BGMVOL] = 100;
  ScrWork[SW_SEVOL] = 100;
  ScrWork[SW_SEVOL + 1] = 100;
  ScrWork[SW_SEVOL + 2] = 100;
  ScrWork[SW_MOVIEALPHA] = 256;

  if (Profile::Vm::GameInstructionSet == +InstructionSet::RNE ||
      Profile::Vm::GameInstructionSet == +InstructionSet::Dash) {
    ScrWork[SW_IRUOCAMERAHFOVCUR] = 40000;
    ScrWork[SW_MAINCAMERAHFOVCUR] = 40000;
    ScrWork[SW_AR_POSX] = 640;
    ScrWork[SW_AR_POSY] = 360;
    // Hack for now
    ScrWork[SW_AR_ELVMIN] = -14674;
    ScrWork[SW_AR_ELVMAX] = 13974;
    ScrWork[SW_AR_ROTMIN] = -19588;
    ScrWork[SW_AR_ROTMAX] = 19088;
  }

  if (Profile::Vm::GameInstructionSet == +InstructionSet::MO7) {
    ScrWork[SW_SHORTCUT] = 0xFFFF;
  }

  ScrWork[SW_SINSTALL_ALL] = 100;
}
VmInstruction(InstSaveSlot) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SaveSlot(arg1: {:d}, arg2: {:d})\n", arg1, arg2);
}
VmInstruction(InstSystemMain) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SystemMain()\n");
}
VmInstruction(InstGameInfoInit) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction GameInfoInit(arg1: {:d})\n", arg1);
}
VmInstruction(InstSystemDataReset) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SystemDataReset(arg1: {:d})\n", arg1);
}
VmInstruction(InstDebugData) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction DebugData(arg1: {:d})\n", arg1);
}
VmInstruction(InstAutoSave) {
  StartInstruction;
  auto quickSave = [&](int autosaveRestartCheck, int saveType) {
    if (ScrWork[SW_AUTOSAVERESTART] != autosaveRestartCheck) {
      int quicksaveEntries = SaveSystem::GetQuickSaveOpenSlot();
      if (quicksaveEntries != -1) {
        SaveIconDisplay::ShowFor(2.4f);
        SaveSystem::FlushWorkingSaveEntry(SaveSystem::SaveType::SaveQuick,
                                          quicksaveEntries, saveType);
      }
    }
    SetFlag(1285, 1);
    ScrWork[SW_AUTOSAVERESTART] = 0;
  };

  PopUint8(type);
  switch (type) {
    case 0:  // QuickSave
      if (ScrWork[SW_TITLE] == 0xffff) break;
      SaveSystem::SaveMemory();
      [[fallthrough]];
    case 20:
      if (ScrWork[SW_TITLE] == 0xffff) break;
      quickSave(1, 1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction AutoSave(type: QuickSave)\n");
      break;
    case 1:  // AutoSaveRestart (?)
      if (ScrWork[SW_TITLE] == 0xffff) break;
      SaveSystem::SaveMemory();
      [[fallthrough]];
    case 21:
      if (ScrWork[SW_TITLE] == 0xffff) break;
      quickSave(3, 3);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction AutoSave(type: {:d})\n", type);
      break;
    case 3: {  // DisableAutoSave
      int quicksaveEntries = SaveSystem::GetQuickSaveOpenSlot();
      if (quicksaveEntries != -1) {
        SaveIconDisplay::ShowFor(2.4f);
        SaveSystem::FlushWorkingSaveEntry(SaveSystem::SaveType::SaveQuick,
                                          quicksaveEntries, 0);
      }

      SetFlag(1285, 0);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction AutoSave(type: {:d})\n", type);
    } break;
    case 5: {  // EnableAutoSave
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction AutoSave(type: {:d})\n", type);
      if (ScrWork[SW_TITLE] != 0xffff) {
        SaveSystem::SaveMemory();
        SetFlag(1285, 1);
        ScrWork[SW_AUTOSAVERESTART] = 0;
      }
    } break;
    case 10: {  // SetCheckpointId
      if (Profile::Vm::UseReturnIds) {
        PopUint16(checkpointId);
        SaveSystem::SetCheckpointId(checkpointId);
        ImpLogSlow(
            LogLevel::Warning, LogChannel::VMStub,
            "STUB instruction AutoSave(type: {:d}, checkpointId: {:d})\n", type,
            checkpointId);
      } else {
        ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                   "STUB instruction AutoSave(type: {:d})\n", type);
      }
    } break;
    case 2:
    case 4:
    case 6:
    case 7:
    case 8:
    case 9:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction AutoSave(arg1: {:d})\n", type);
      break;
    case 0xff: {
      BlockThread;
    } break;
    default:
      // More quicksave cases here
      break;
  }
}
VmInstruction(InstLoadFontWidths) {
  StartInstruction;
  PopExpression(fontId);
  PopExpression(archiveId);
  PopExpression(fileId);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction LoadFontWidths(fontId: {:d}, archiveId: {:d}, "
             "fileId: {:d})\n",
             fontId, archiveId, fileId);
}

}  // namespace Vm

}  // namespace Impacto