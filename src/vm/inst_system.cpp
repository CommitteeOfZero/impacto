#include "inst_system.h"

#include "inst_macros.inc"

#include <math.h>
#include <ctime>

#include "expression.h"
#include "../profile/scriptvars.h"
#include "../game.h"
#include "../mem.h"
#include "../log.h"
#include "../audio/audiosystem.h"
#include "../audio/audiostream.h"
#include "../audio/audiochannel.h"
#include "../background2d.h"
#include "../character2d.h"
#include "../text.h"
#include "../profile/dialogue.h"
#include "../profile/vm.h"
#include "../profile/ui/sysmesbox.h"
#include "../inputsystem.h"
#include "interface/input.h"
#include "../savesystem.h"
#include "../window.h"
#include "../ui/ui.h"

namespace Impacto {

namespace Vm {

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
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction KillThread(threadId: %i)\n", threadId);
}
VmInstruction(InstReset) {
  StartInstruction;
  BlockThread;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Reset()\n");
}
VmInstruction(InstScriptLoad) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(scriptId);
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
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction GetLabelAdr(unknown)\n");
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
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction KeyWait(type: %i, arg1: %i, arg2: %i)\n", type,
             arg1, arg2);
}
VmInstruction(InstKeyWaitTimer) {
  StartInstruction;
  PopUint8(type);
  PopExpression(timer);
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction KeyWaitTimer(type: %i, timer: %i, arg1: %i, "
             "arg2: %i)\n",
             type, timer, arg1, arg2);
}
VmInstruction(InstMemberWrite) {
  StartInstruction;
  if (noExpressions) {
    PopUint8(index);
    uint32_t* thdElement = (uint32_t*)thread->GetMemberPointer(index);
    uint8_t* immValue = thread->Ip;
    int value = SDL_SwapLE32(immValue[0] + (immValue[1] << 8) +
                             (immValue[2] << 16) + (immValue[3] << 24));
    thread->Ip += 4;
    *thdElement = value;
  } else {
    PopExpression(index);
    uint32_t* thdElement = (uint32_t*)thread->GetMemberPointer(index);
    PopExpression(value);
    *thdElement = value;
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
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction ThreadControlStore(type: %i)\n", type);
}
VmInstruction(InstPadAct) {
  StartInstruction;
  PopExpression(unused);
  PopExpression(vib1);
  PopExpression(vib2);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction PadAct(unused: %i, vib1: %i, vib2: %i)\n",
             unused, vib1, vib2);
}
VmInstruction(InstCopyThreadWork) {
  StartInstruction;
  PopExpression(threadIdSrc);
  PopExpression(threadIdDst);
  PopExpression(beginIndex);
  PopExpression(count);
  Sc3VmThread* srcThread = &ThreadPool[threadIdSrc];
  Sc3VmThread* dstThread;
  if (threadIdDst == 0)
    dstThread = thread;
  else
    dstThread = &ThreadPool[threadIdSrc];

  for (int i = 0; i < count; i++) {
    dstThread->Variables[beginIndex + i] = srcThread->Variables[beginIndex + i];
  }
}
VmInstruction(InstSave) {
  StartInstruction;
  PopUint8(type);
  switch (type) {  // TODO: Types 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                   // 16, 20, 21, 72, 30, 31, 32, 33, 34, 35, 41, 50, 51, 66,
                   // 67, 70, 71, 74, 76
    case 32:
      ScrWork[SW_SAVEERRORCODE] = SaveSystem::MountSaveFile();
      break;
    case 40:  // SystemDataCheck
      if (Profile::Vm::GameInstructionSet == +InstructionSet::RNE) {
        PopExpression(unused1);
        PopExpression(unused2);
        PopExpression(unused3);
        PopExpression(unused4);
      }
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Save(type: %i)\n",
                 type);
      break;
    default:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Save(type: %i)\n",
                 type);
      break;
  }
}
VmInstruction(InstSaveIconLoad) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SaveIconLoad(arg1: %i)\n",
             arg1);
}
VmInstruction(InstVoiceTableLoadMaybe) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction VoiceTableLoad(arg1: %i)\n", arg1);
}
VmInstruction(InstSetPadCustom) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SetPadCustom()\n");
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
  // TODO: Implement this when we have skip mode
  // if (SkipModeEnabled) {
  //  thread->WaitCounter = 0;
  //}
  if (thread->WaitCounter > 0) {
    ResetInstruction;
    BlockThread;
  }
}
VmInstruction(InstTerminate) {
  StartInstruction;

  ImpLog(LL_Info, LC_VM, "VM requested shutdown!\n");
  Window::Shutdown();
  // BlockThread;
  // ResetInstruction;
}
VmInstruction(InstDebugPrint) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LL_Debug, LC_VM, "DebugPrint(arg1: %i, arg2: %i)\n", arg1, arg2);
}
VmInstruction(InstSystemMes) {
  StartInstruction;
  PopUint8(mode);
  switch (Profile::Vm::GameInstructionSet) {
    case InstructionSet::Dash:
    case InstructionSet::CC:
      PopUint8(unk01);
      break;
  }

  switch (mode) {
    case 0:  // SystemMesInit0
    case 1:  // SystemMesInit1
             // SysMesBox::Implementation->ChoiceMade = false;
             // SysMesBox::Implementation->CurrentChoice = 255;
             // SysMesBox::Implementation->MessageCount = 0;
             // memset(SysMesBox::Implementation->Messages, 0,
             //       (8 * 255) * sizeof(ProcessedTextGlyph));
             // SysMesBox::Implementation->ChoiceCount = 0;
             // memset(SysMesBox::Implementation->Choices, 0,
             //       (8 * 255) * sizeof(ProcessedTextGlyph));

      UI::SysMesBoxPtr->Init();
      break;
    case 2: {  // SystemMesInit2
      PopExpression(sysMesInit2Arg);
      // ScrWork[SW_SYSMESANIMCTF] =
      //    2 * SysMesBox::Implementation->MessageCount + 33;
      ScrWork[SW_SYSMESANIMCTF] = 2 * UI::SysMesBoxPtr->MessageCount + 33;
    } break;
    case 3: {  // SystemMesSetMes
      PopUint16(sysMesStrNum);

      UI::SysMesBoxPtr->AddMessage(ScriptGetStrAddress(
          ScriptBuffers[thread->ScriptBufferId], sysMesStrNum));
      // uint8_t* oldIp = thread->Ip;
      // thread->Ip = ScriptGetStrAddress(ScriptBuffers[thread->ScriptBufferId],
      //                                 sysMesStrNum);
      // int len = TextLayoutPlainLine(
      //    thread, 255,
      //    SysMesBox::Implementation
      //        ->Messages[SysMesBox::Implementation->MessageCount],
      //    Profile::Dialogue::DialogueFont, Profile::SysMesBox::TextFontSize,
      //    Profile::Dialogue::ColorTable[10], 1.0f,
      //    glm::vec2(Profile::SysMesBox::TextX, 0.0f), TextAlignment::Left);
      // float mesLen = 0.0f;
      // for (int i = 0; i < len; i++) {
      //  mesLen += SysMesBox::Implementation
      //                ->Messages[SysMesBox::Implementation->MessageCount][i]
      //                .DestRect.Width;
      //}
      // SysMesBox::Implementation
      //    ->MessageWidths[SysMesBox::Implementation->MessageCount] = mesLen;
      // SysMesBox::Implementation
      //    ->MessageLengths[SysMesBox::Implementation->MessageCount] = len;
      // SysMesBox::Implementation->MessageCount++;
      // thread->Ip = oldIp;
    } break;
    case 4: {  // SystemMesSetSel
      PopUint16(sysSelStrNum);

      UI::SysMesBoxPtr->AddChoice(ScriptGetStrAddress(
          ScriptBuffers[thread->ScriptBufferId], sysSelStrNum));
      // uint8_t* oldIp = thread->Ip;
      // thread->Ip = ScriptGetStrAddress(ScriptBuffers[thread->ScriptBufferId],
      //                                 sysSelStrNum);
      // int len = TextLayoutPlainLine(
      //    thread, 255,
      //    SysMesBox::Implementation
      //        ->Choices[SysMesBox::Implementation->ChoiceCount],
      //    Profile::Dialogue::DialogueFont, Profile::SysMesBox::TextFontSize,
      //    Profile::Dialogue::ColorTable[10], 1.0f,
      //    glm::vec2(Profile::SysMesBox::TextX, 0.0f), TextAlignment::Left);
      // float mesLen = 0.0f;
      // for (int i = 0; i < len; i++) {
      //  mesLen += SysMesBox::Implementation
      //                ->Choices[SysMesBox::Implementation->ChoiceCount][i]
      //                .DestRect.Width;
      //}
      // SysMesBox::Implementation
      //    ->ChoiceWidths[SysMesBox::Implementation->ChoiceCount] = mesLen;
      // SysMesBox::Implementation
      //    ->ChoiceLengths[SysMesBox::Implementation->ChoiceCount] = len;
      // SysMesBox::Implementation->ChoiceCount++;
      // thread->Ip = oldIp;
    } break;
    case 5:  // SystemMesMain
             // if (!SysMesBox::Implementation->ChoiceMade) {
             //  ResetInstruction;
             //  BlockThread;
             //} else {
             //  ScrWork[SW_SYSSEL] = SysMesBox::Implementation->CurrentChoice;
             //}

      if (!UI::SysMesBoxPtr->ChoiceMade) {
        ResetInstruction;
        BlockThread;
      }
      break;
    case 6:  // SystemMesFadeIn
             // ScrWork[SW_SYSMESALPHA] = 256;
             // SysMesBox::Implementation->BoxOpacity = 1.0f;
             // SysMesBox::Show();
             // if (ScrWork[SW_SYSMESANIMCTCUR] < ScrWork[SW_SYSMESANIMCTF]) {
             //  ResetInstruction;
             //  BlockThread;
             // }

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

      // SysMesBox::Hide();
      // if (ScrWork[SW_SYSMESANIMCTCUR] > 0) {
      //  ResetInstruction;
      //  BlockThread;
      // } else {
      //  ScrWork[SW_SYSMESALPHA] = 0;
      //  SysMesBox::Implementation->BoxOpacity = 0.0f;
      // }
      break;
    case 8:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SystemMes(mode: %i)\n", mode);
      break;
    case 20:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SystemMes(mode: %i)\n", mode);
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
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction GetSystemStatus(type: %i)\n", type);
}
VmInstruction(InstReboot) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Reboot()\n");
  BlockThread;
}
VmInstruction(InstReloadScript) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction ReloadScript()\n");
  BlockThread;
  ResetInstruction;
}
VmInstruction(InstReloadScriptMenu) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction ReloadScriptMenu(arg1: %i)\n", arg1);
}
VmInstruction(InstDebugEditer) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction DebugEditer(arg1: %i)\n",
             arg1);
}
VmInstruction(InstPadActEx) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction PadActEx(arg1: %i, arg2: %i, arg3: "
             "%i)\n",
             arg1, arg2, arg3);
}
VmInstruction(InstDebugSetup) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction DebugSetup(arg1: %i)\n",
             arg1);
}
VmInstruction(InstGlobalSystemMessage) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction GlobalSystemMessage(arg1: %i)\n", arg1);
}
// TODO find the value ranges for atan2
VmInstruction(InstCalc) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // CalcSin
      PopExpression(dest);
      PopExpression(angle);
      ScrWork[dest] = (std::sin(angle / (65536 / (2 * M_PI)))) * 65536;
    } break;
    case 1: {  // CalcCos
      PopExpression(dest);
      PopExpression(angle);
      ScrWork[dest] = (std::cos(angle / (65536 / (2 * M_PI)))) * 65536;
    } break;
    case 2: {  // CalcAtan2
      PopExpression(dest);
      PopExpression(x);
      PopExpression(y);
      ScrWork[dest] = atan2(x, y);
    } break;
    case 3: {  // CalcSinL
      PopExpression(dest);
      PopExpression(base);
      PopExpression(angle);
      PopExpression(offset);
      ScrWork[dest] =
          offset + base * ((std::sin(angle / (65536 / (2 * M_PI)))) * 65536);
    } break;
    case 4: {  // CalcCosL
      PopExpression(dest);
      PopExpression(base);
      PopExpression(angle);
      PopExpression(offset);
      ScrWork[dest] =
          offset + base * ((std::cos(angle / (65536 / (2 * M_PI)))) * 65536);
    } break;
    case 5: {  // CalcRound
      PopExpression(dest);
      PopExpression(value);
      PopExpression(multiplier);
      PopExpression(divider);
      ScrWork[dest] =
          (((((multiplier * value) * 10.0) / divider) + 5.0) / 10.0);
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
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MSinit(initType: %i)\n",
             initType);
  for (int i = 0; i < MaxBackgrounds2D; i++) {
    ScrWork[SW_BG1SURF + i] = i;
  }
  for (int i = 0; i < MaxCharacters2D; i++) {
    ScrWork[SW_CHA1SURF + i] = i;
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

  ScrWork[SW_IRUOCAMERAHFOVCUR] = 40000;
  ScrWork[SW_MAINCAMERAHFOVCUR] = 40000;
  ScrWork[SW_AR_POSX] = 640;
  ScrWork[SW_AR_POSY] = 360;
  // Hack for now
  ScrWork[SW_AR_ELVMIN] = -14674;
  ScrWork[SW_AR_ELVMAX] = 13974;
  ScrWork[SW_AR_ROTMIN] = -19588;
  ScrWork[SW_AR_ROTMAX] = 19088;

  if (Profile::Vm::GameInstructionSet == +InstructionSet::MO7) {
    ScrWork[SW_SHORTCUT] = 0xFFFF;
  }
}
VmInstruction(InstSaveSlot) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction SaveSlot(arg1: %i, arg2: %i)\n", arg1, arg2);
}
VmInstruction(InstSystemMain) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SystemMain()\n");
}
VmInstruction(InstGameInfoInit) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction GameInfoInit(arg1: %i)\n",
             arg1);
}
VmInstruction(InstSystemDataReset) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction SystemDataReset(arg1: %i)\n", arg1);
}
VmInstruction(InstDebugData) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction DebugData(arg1: %i)\n",
             arg1);
}
VmInstruction(InstAutoSave) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // QuickSave
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction AutoSave(type: QuickSave)\n");
      break;
    case 1:  // AutoSaveRestart (?)
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction AutoSave(type: %i)\n",
                 type);
      break;
    case 3:  // DisableAutoSave
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction AutoSave(type: %i)\n",
                 type);
      break;
    case 5:  // EnableAutoSave
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction AutoSave(type: %i)\n",
                 type);
      break;
    case 10: {  // SetCheckpointId
      if (Profile::Vm::UseReturnIds) {
        PopUint16(checkpointId);
        ImpLogSlow(LL_Warning, LC_VMStub,
                   "STUB instruction AutoSave(type: %i, checkpointId: %i)\n",
                   type, checkpointId);
      } else {
        ImpLogSlow(LL_Warning, LC_VMStub,
                   "STUB instruction AutoSave(type: %i)\n", type);
      }
    } break;
    case 2:
    case 4:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction AutoSave(arg1: %i)\n",
                 type);
      break;
  }
}
VmInstruction(InstLoadFontWidths) {
  StartInstruction;
  PopExpression(fontId);
  PopExpression(archiveId);
  PopExpression(fileId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction LoadFontWidths(fontId: %i, archiveId: %i, "
             "fileId: %i)\n",
             fontId, archiveId, fileId);
}

}  // namespace Vm

}  // namespace Impacto