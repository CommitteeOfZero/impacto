#include "inst_dialogue.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../profile/scriptvars.h"
#include "../profile/configsystem.h"
#include "../mem.h"
#include "../log.h"
#include "../audio/audiostream.h"
#include "../profile/vm.h"
#include "../hud/saveicondisplay.h"
#include "../hud/tipsnotification.h"
#include "../data/savesystem.h"
#include "../data/tipssystem.h"
#include "../ui/ui.h"
#include "interface/input.h"
#include "../text.h"
#include "vm.h"

namespace Impacto {

namespace Vm {

using namespace Impacto::Profile::ScriptVars;

VmInstruction(InstMesViewFlag) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // Set
      PopExpression(scriptId);
      PopExpression(lineId);
      SaveSystem::SetLineRead(scriptId, lineId);
    } break;
    case 1: {  // Check
      PopExpression(scrWorkEntry);
      PopExpression(scriptId);
      PopExpression(lineId);
      ScrWork[scrWorkEntry] = SaveSystem::IsLineRead(scriptId, lineId);
    } break;
  }
}
VmInstruction(InstSetMesWinPri) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  PopExpression(unused);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction MesViewFlag(arg1: {:d}, arg2: "
             "{:d}, unused: {:d})\n",
             arg1, arg2, unused);
}
VmInstruction(InstMesSync) {
  StartInstruction;

  PopUint8(type);
  switch (type) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 11:
    case 12:
    case 13:
    case 14:
    case 20:
      break;
    case 10: {
      PopExpression(unknown);
    } break;
  }

  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction MesSync(type: {:d})\n", type);
}
VmInstruction(InstMesSetID) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // SetSavePointPage0
      if (Profile::Vm::UseReturnIds) {
        PopUint16(savePointId);
        if (!GetFlag(SF_MESSAVEPOINT_SSP)) {
          SaveSystem::SetCheckpointId(savePointId);
        }
        ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                   "STUB instruction MesSetID(type: SetSavePoint, savePointId: "
                   "{:d})\n",
                   savePointId);
      } else {
        ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                   "STUB instruction MesSetID(type: SetSavePoint)\n");
      }
    } break;
    case 1: {  // SetSavePointForPage
      PopUint16(savePointId);
      PopExpression(dialoguePageId);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MesSetID(type: SetSavePoint1, "
                 "savePointId: {:d}, arg1: {:d})\n",
                 savePointId, dialoguePageId);
      if (!GetFlag(SF_MESSAVEPOINT_SSP + dialoguePageId)) {
        SaveSystem::SetCheckpointId(savePointId);
      }
      thread->DialoguePageId = dialoguePageId;
    } break;
    case 2: {  // SetPage
      PopExpression(dialoguePageId);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction MesSetID(type: SetPage, dialoguePageId: {:d})\n",
          dialoguePageId);
      thread->DialoguePageId = dialoguePageId;
    } break;
  }
}
VmInstruction(InstMesCls) {
  StartInstruction;
  PopUint8(type);  // TODO: Implement types 0, 1, 2, 3, 4, 5, 6, 7, 8
  if ((type & 0xFE) != 4 && !(type & 1)) {
    PopExpression(arg1);
    ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
               "STUB instruction MesCls(type: {:d}, arg1: {:d})\n", type, arg1);
  } else {
    ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
               "STUB instruction MesCls(type: {:d})\n", type);
  }
  switch (type) {
    case 1: {
      for (int i = 0; i < DialoguePageCount; i++) {
        DialoguePages[i].Clear();
      }
      SetFlag(SF_SHOWWAITICON, 0);
      SetFlag(SF_SHOWWAITICON + 1, 0);
      SetFlag(SF_SHOWWAITICON + 2, 0);
    } break;
  }
}
VmInstruction(InstMesVoiceWait) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction MesVoiceWait()\n");
}
VmInstruction(InstMes) {
  StartInstruction;

  DialoguePage& dialoguePage = DialoguePages[thread->DialoguePageId];
  uint32_t scriptId = LoadedScriptMetas[thread->ScriptBufferId].Id;

  // After loading a save we need to make sure the textbox is actually shown
  if (dialoguePage.FadeAnimation.IsOut() &&
      GetFlag(thread->DialoguePageId + SF_MESWINDOW0OPENFL)) {
    dialoguePage.Mode =
        (DialoguePageMode)ScrWork[thread->DialoguePageId * 10 + SW_MESMODE0];
    dialoguePage.FadeAnimation.StartIn(true);
  }

  PopUint8(type);
  bool voiced = type & 1;
  bool acted = type & (1 << 1);
  bool MSB = type & (1 << 7);

  int audioId = -1;
  int animationId = 0;
  if (voiced) audioId = ExpressionEval(thread);
  if (acted) animationId = ExpressionEval(thread);
  PopExpression(characterId);
  if (characterId >= 32) characterId = 0;
  PopUint16(lineId);
  uint32_t line = MSB ? MsbGetStrAddress(thread->ScriptBufferId, lineId)
                      : ScriptGetStrAddress(thread->ScriptBufferId, lineId);

  if (!(ScrWork[10 * thread->DialoguePageId + SW_MESWIN0TYPE] & (1 << 6))) {
    SetFlag(SF_MESREAD, SaveSystem::IsLineRead(scriptId, lineId));
    ChkMesSkip();
  }

  ScrWork[2 * dialoguePage.Id + SW_LINEID] = lineId;
  ScrWork[2 * dialoguePage.Id + SW_SCRIPTID] = scriptId;
  ScrWork[dialoguePage.Id + SW_ANIME0CHANO] = characterId;

  Audio::AudioStream* audioStream = nullptr;
  if (voiced) {
    Io::Stream* stream;
    IoError err = Io::VfsOpen("voice", audioId, &stream);

    bool playAudio = (err == IoError_OK && !GetFlag(SF_MESALLSKIP));
    if (playAudio) audioStream = Audio::AudioStream::Create(stream);
  }

  uint32_t oldIp = thread->IpOffset;
  thread->IpOffset = line;
  dialoguePage.AddString(thread, audioStream, animationId);
  ResetInstruction;
  if (!GetFlag(SF_MESSAVEPOINT_SSP + thread->DialoguePageId)) {
    if ((ScrWork[thread->DialoguePageId * 10 + SW_MESWIN0TYPE] & 4) == 0 &&
        ScrWork[SW_TITLE] != 0xffff) {
      SaveSystem::SaveMemory();
      SetFlag(SF_SAVECAPTURE, 1);
      SetFlag(SF_AUTOSAVEENABLE, 1);
    }
  } else {
    SetFlag(SF_MESSAVEPOINT_SSP + thread->DialoguePageId, 0);
  }

  thread->IpOffset = oldIp;
  UI::BacklogMenuPtr->AddMessage(
      {.ScriptBufferId = thread->ScriptBufferId, .IpOffset = line}, audioId,
      characterId);
}
VmInstruction(InstMesMain) {
  StartInstruction;
  PopUint8(type);
  DialoguePage* currentPage = &DialoguePages[thread->DialoguePageId];

  bool advanceButtonWentDown =
      Interface::PADinputButtonWentDown & Interface::PAD1A ||
      Interface::PADinputMouseWentDown & Interface::PAD1A ||
      (Profile::ConfigSystem::AdvanceTextOnDirectionalInput &&
       Interface::PADinputButtonWentDown &
           (Interface::PAD1UP | Interface::PAD1DOWN | Interface::PAD1LEFT |
            Interface::PAD1RIGHT));

  if (type == 0) {  // Normal mode
    if (!currentPage->TextIsFullyOpaque()) {
      // Text is still appearing
      if (advanceButtonWentDown || GetFlag(SF_MESALLSKIP)) {
        if (!currentPage->Typewriter.IsCancelled) {
          currentPage->Typewriter.CancelRequested = true;
        }
      }
    } else {
      // Text is fully opaque
      SetFlag(SF_SHOWWAITICON + thread->DialoguePageId, true);

      if (!GetFlag(SF_UIHIDDEN)) {
        if (advanceButtonWentDown || GetFlag(SF_MESALLSKIP) ||
            !currentPage->AutoWaitTime) {
          // Advance to next line
          SaveSystem::SetLineRead(ScrWork[2 * currentPage->Id + SW_SCRIPTID],
                                  ScrWork[2 * currentPage->Id + SW_LINEID]);
          SetFlag(SF_SHOWWAITICON + thread->DialoguePageId, false);

          if (Profile::ConfigSystem::SkipVoice || GetFlag(SF_MESALLSKIP))
            Audio::Channels[Audio::AC_VOICE0]->Stop(0.0f);

          BlockThread;
          return;
        }
      }
    }

    ResetInstruction;
    BlockThread;
  }
  // TODO: Type 1 - Skip mode(?)
}
VmInstruction(InstSetMesModeFormat) {
  StartInstruction;
  PopExpression(id);
  PopLocalLabel(modeDataAdr);
  (void)modeDataAdr;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SetMesModeFormat(id: {:d})\n", id);
}
VmInstruction(InstSetNGmoji) {
  StartInstruction;
  PopString(strAdr1);
  PopString(strAdr2);
  (void)strAdr1;
  (void)strAdr2;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SetNGmoji()\n");
}
VmInstruction(InstMesRev) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // DispInit
      UI::BacklogMenuPtr->Show();
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetMesModeFormat(type: DispInit)\n");
      break;
    case 1:  // Main
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetMesModeFormat(type: Main)\n");
      break;
    case 2:  // AllCls
      UI::BacklogMenuPtr->Hide();
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetMesModeFormat(type: AllCls)\n");
      break;
    case 3:  // ChkLoad
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetMesModeFormat(type: ChkLoad)\n");
      break;
    case 4:  // SAVELoad
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetMesModeFormat(type: SAVELoad)\n");
      break;
    case 5:  // SoundUnk
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetMesModeFormat(type: SoundUnk)\n");
      break;
    case 0xA:  // DispInit
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetMesModeFormat(type: DispInit)\n");
      break;
  }
}
VmInstruction(InstMessWindow) {
  StartInstruction;
  PopUint8(type);
  DialoguePage* currentPage = &DialoguePages[thread->DialoguePageId];
  switch (type) {
    case 0:  // HideCurrent
      if (!currentPage->FadeAnimation.IsOut()) {
        currentPage->FadeAnimation.StartOut();
        SetFlag(thread->DialoguePageId + SF_MESWINDOW0OPENFL, 0);
      }
      break;
    case 1:  // ShowCurrent
      if (!currentPage->FadeAnimation.IsIn()) {
        currentPage->Mode = (DialoguePageMode)
            ScrWork[thread->DialoguePageId * 10 + SW_MESMODE0];
        currentPage->FadeAnimation.StartIn(true);
        SetFlag(thread->DialoguePageId + SF_MESWINDOW0OPENFL, 1);
      }
      break;
    case 2:  // AwaitShowCurrent
      if (currentPage->FadeAnimation.State == AnimationState::Playing) {
        ResetInstruction;
        BlockThread;
      }
      break;
    case 3:  // AwaitHideCurrent
      if (currentPage->FadeAnimation.State == AnimationState::Playing) {
        ResetInstruction;
        BlockThread;
      } else if (currentPage->FadeAnimation.IsOut()) {
        currentPage->Clear();
      }
      break;
    case 4:  // HideCurrent04
      if (!currentPage->FadeAnimation.IsOut()) {
        currentPage->FadeAnimation.StartOut();
        SetFlag(thread->DialoguePageId + SF_MESWINDOW0OPENFL, 0);
      }
      break;
    case 5: {  // Hide
      PopExpression(messWindowId);
      if (!DialoguePages[messWindowId].FadeAnimation.IsOut()) {
        DialoguePages[messWindowId].FadeAnimation.StartOut();
        SetFlag(messWindowId + SF_MESWINDOW0OPENFL, 0);
      }
    } break;
    case 6: {  // HideSlow
      PopExpression(messWindowId);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction MessWindow(type: HideSlow, messWindowId: {:d})\n",
          messWindowId);
    } break;
    case 7: {  // HideSlow
      PopExpression(messWindowId);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction MessWindow(type: HideSlow, messWindowId: {:d})\n",
          messWindowId);
    } break;
  }
}
VmInstruction(InstSel) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // SelInit
      if (Profile::Vm::GameInstructionSet == +InstructionSet::Dash ||
          Profile::Vm::GameInstructionSet == +InstructionSet::CC ||
          Profile::Vm::GameInstructionSet == +InstructionSet::MO8) {
        PopUint16(savepointid);
        // SF_MESSAVEPOINT_SSP + dialog page's field 5 in decompile?
        // if (GetFlag(SF_MESSAVEPOINT_SSP + thread->DialoguePageId) == 0) {
        if (ScrWork[SW_TITLE] != 0xffff) {
          SaveSystem::SetCheckpointId(savepointid);
          SaveSystem::SaveMemory();
          SetFlag(SF_SAVECAPTURE, 1);
          SetFlag(SF_AUTOSAVEENABLE, 1);
          BlockThread;
        }
        // }
      }
      PopExpression(arg1);
      UI::SelectionMenuPtr->InitSelectionMenu((bool)arg1);
      // SaveIconDisplay::Show();
      break;
    }
    case 1: {
      PopUint16(selStrNum);
      auto offset = ScriptGetStrAddress(thread->ScriptBufferId, selStrNum);
      UI::SelectionMenuPtr->AddChoice(
          {.ScriptBufferId = thread->ScriptBufferId, .IpOffset = offset});
      break;
    }
    case 0x81: {
      PopMsbString(line);
      UI::SelectionMenuPtr->AddChoice(
          {.ScriptBufferId = thread->ScriptBufferId, .IpOffset = line});
    } break;
    case 2: {
      PopUint16(selStrNum);
      auto offset = ScriptGetStrAddress(thread->ScriptBufferId, selStrNum);
      UI::SelectionMenuPtr->AddChoice(
          {.ScriptBufferId = thread->ScriptBufferId, .IpOffset = offset});
      PopExpression(arg2);
      break;
    }
    case 0x82: {
      PopMsbString(line);
      PopExpression(arg2);
      UI::SelectionMenuPtr->AddChoice(
          {.ScriptBufferId = thread->ScriptBufferId, .IpOffset = line});
    } break;
  }
}
VmInstruction(InstSelect) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      UI::SelectionMenuPtr->Show();
      bool flag = GetFlag(SF_SAVEDISABLE);
      SetFlag(thread->DialoguePageId + 1213, 0);
      if (ScrWork[SW_AUTOSAVERESTART] == 2) {
        thread->IpOffset += 12;
        return;
      } else {
        SaveSystem::SaveMemory();
        int quicksaveEntries = SaveSystem::GetQuickSaveOpenSlot();
        if (!flag && quicksaveEntries != -1) {
          SaveIconDisplay::ShowFor(2.4f);
          SaveSystem::FlushWorkingSaveEntry(SaveSystem::SaveType::Quick,
                                            quicksaveEntries, 2);
          SaveSystem::SaveThumbnailData();
        }
        ScrWork[SW_AUTOSAVERESTART] = 0;
        if (quicksaveEntries == -1) {
          thread->IpOffset += 12;
        }
      }
      SetFlag(SF_SYSMENUDISABLE, 0);
    } break;
    case 1: {
      if (!UI::SelectionMenuPtr->ChoiceMade) {
        ResetInstruction;
        BlockThread;
      } else if (UI::SelectionMenuPtr->State == UI::MenuState::Hiding) {
        ResetInstruction;
        BlockThread;
      } else if (UI::SelectionMenuPtr->State == UI::MenuState::Hidden) {
        BlockThread;
      } else {
        UI::SelectionMenuPtr->Hide();
        ResetInstruction;
        BlockThread;
      }
    } break;
    case 2: {
      PopExpression(arg1);
      ScrWork[arg1] = UI::SelectionMenuPtr->SelectedChoiceId;
    } break;
  }
}
VmInstruction(InstSysSel) {
  StartInstruction;
  PopUint8(type);  // TODO: Implement type 0, 1
  if (type >= 2) {
    PopString(arg1);
    (void)arg1;
  }
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SysSel(type: {:d})\n", type);
}
VmInstruction(InstSysSelect) {
  StartInstruction;
  PopUint8(type);
  switch (type & 0xF) {
    case 0: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction SysSelect(type: {:d}, arg1: {:d}, arg2: {:d})\n",
          type, arg1, arg2);
    } break;
    case 2:
    case 3: {
      PopExpression(destination);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SysSelect(type: {:d}, destination: {:d})\n",
                 type, destination);
    } break;
  }
}
VmInstruction(InstSetTextTable) {
  StartInstruction;
  PopExpression(id);
  PopLocalLabel(tableDataAdr);
  TextTable[id].scriptBufferId = static_cast<uint8_t>(thread->ScriptBufferId);
  TextTable[id].labelAdr = tableDataAdr;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SetTextTable(id: {:d})\n", id);
}
VmInstruction(InstSetDic) {
  StartInstruction;
  PopUint8(type);
  PopExpression(tipId);
  switch (type) {
    case 0:    // NewTip
    case 1: {  // Check
      bool tipLocked = TipsSystem::GetTipLockedState(tipId);
      if (tipLocked) {
        TipsSystem::SetTipLockedState(tipId, false);
        TipsSystem::SetTipNewState(tipId, true);
        TipsSystem::SetTipUnreadState(tipId, true);
        TipsNotification::AddTip(tipId);
        TipsSystem::GetNewTipsIndices().push_back(static_cast<uint16_t>(tipId));
      }
      if (type == 1) {
        PopExpression(flagId);
        SetFlag(flagId, tipLocked);
      }
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetDic(type: NewTip, tipId: {:d})\n", tipId);
    } break;
    case 2:  // SetDic02
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetDic(type: {:d}, tipId: {:d})\n", type,
                 tipId);
      break;
  }
}
VmInstruction(InstEncyclopedia) {
  StartInstruction;
  PopExpression(tipId);
  if (TipsSystem::GetTipLockedState(tipId)) {
    TipsSystem::SetTipLockedState(tipId, false);
    TipsNotification::AddTip(tipId);
    TipsSystem::GetNewTipsIndices().push_back(static_cast<uint16_t>(tipId));
  }
}
VmInstruction(InstNameID) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:
      if (Profile::Vm::GameInstructionSet == +InstructionSet::CC ||
          Profile::Vm::GameInstructionSet == +InstructionSet::MO8 ||
          Profile::Vm::GameInstructionSet == +InstructionSet::CHN) {
        PopLocalLabel(namePlateDataBlock);
        Sc3Stream namePlateData(
            &ScriptBuffers[thread->ScriptBufferId][namePlateDataBlock]);
        if (!Profile::Vm::UseMsbStrings) InitNamePlateData(namePlateData);
      } else if (Profile::Vm::GameInstructionSet == +InstructionSet::MO6TW) {
        PopExpression(arg1);
        PopExpression(arg2);
        PopExpression(arg3);
      }
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction NameID(type: {:d})\n", type);
      break;
    case 1: {
      PopLocalLabel(namePlateDataBlock);
      (void)namePlateDataBlock;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction NameID(type: {:d})\n", type);
    } break;
    case 2:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction NameID(type: {:d})\n", type);
      break;
  }
}
VmInstruction(InstTips) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // TipsDataInit
      PopUint16(tipsLabelNum);
      uint32_t tipsDataAdr =
          ScriptGetLabelAddress(thread->ScriptBufferId, tipsLabelNum);
      if (Profile::Vm::GameInstructionSet == +InstructionSet::MO8 ||
          Profile::Vm::GameInstructionSet == +InstructionSet::CHN) {
        PopLocalLabel(tipsDataAdr1);
        (void)tipsDataAdr1;
      }
      uint32_t tipsDataSize =
          ScriptGetLabelSize(thread->ScriptBufferId, tipsLabelNum);
      TipsSystem::DataInit(thread->ScriptBufferId, tipsDataAdr, tipsDataSize);
      if (UI::TipsMenuPtr) {
        UI::TipsMenuPtr->Init();
      }

    } break;
    case 1:  // TipsInit
      TipsSystem::UpdateTipRecords();
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Tips(type: TipsInit)\n");
      break;
    case 2:  // TipsMain
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Tips(type: TipsMain)\n");
      break;
    case 3:  // TipsEnd
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Tips(type: TipsEnd)\n");
      break;
    case 4:  // TipsSet
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Tips(type: TipsSet)\n");
      TipsSystem::UpdateTipRecords();
      break;
    case 10:  // Tips_ProfSetXboxEvent
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Tips(type: Tips_ProfSetXboxEvent)\n");
      break;
  }
}
VmInstruction(InstSetRevMes) {
  StartInstruction;
  PopUint8(type);

  bool voiced = type & (1 << 0);
  bool savep = (type & (1 << 1)) && voiced;
  bool expression = type & (1 << 7);

  if ((type & (1 << 1)) || expression) {
    ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
               "STUB instruction SetRevMes(type: {:d})\n", type);
  }

  int audioId = -1;
  int animationId = 0;
  if (voiced) {
    audioId = ExpressionEval(thread);
    animationId = ExpressionEval(thread);
  }

  if (savep) {
    // TODO: use?
    ExpressionEval(thread);
  }

  int lineId;
  if (expression) {
    lineId = ExpressionEval(thread);
  } else {
    PopUint16(lineIdTemp);
    lineId = lineIdTemp;
  }
  uint32_t line = ScriptGetStrAddress(thread->ScriptBufferId, lineId);

  uint32_t scriptId = LoadedScriptMetas[thread->ScriptBufferId].Id;

  SaveSystem::SetLineRead(scriptId, lineId);
  UI::BacklogMenuPtr->AddMessage(
      {.ScriptBufferId = thread->ScriptBufferId, .IpOffset = line}, audioId,
      animationId);
}

void ChkMesSkip() {
  bool mesSkip = false;

  if (MesSkipMode & (SkipModeFlags::SkipRead | SkipModeFlags::SkipAll)) {
    MesSkipMode &= SkipModeFlags::Auto;
    MesSkipMode |= (Profile::ConfigSystem::SkipRead ? SkipModeFlags::SkipRead
                                                    : SkipModeFlags::SkipAll);
  }

  if (ScrWork[SW_SYSMESALPHA] != 255) MesSkipMode = false;

  if ((ScrWork[SW_GAMESTATE] & 0b101) == 0b001 && !GetFlag(SF_UIHIDDEN)) {
    // Force skip
    mesSkip |=
        (bool)(Interface::PADinputButtonIsDown & Interface::PADcustom[7]);

    // Skip
    if (Interface::PADinputButtonWentDown & Interface::PADcustom[8]) {
      if (MesSkipMode & (SkipModeFlags::SkipRead | SkipModeFlags::SkipAll)) {
        // Turn off all skip modes (leaving auto)
        MesSkipMode &= SkipModeFlags::Auto;
      } else {
        MesSkipMode |=
            (Profile::ConfigSystem::SkipRead ? SkipModeFlags::SkipRead
                                             : SkipModeFlags::SkipAll);
      }
    }

    // Auto
    if (Interface::PADinputButtonWentDown & Interface::PADcustom[9]) {
      MesSkipMode ^= SkipModeFlags::Auto;
    }
  }

  mesSkip |= (bool)(MesSkipMode & SkipModeFlags::SkipAll);
  mesSkip |= (MesSkipMode & SkipModeFlags::SkipRead) && GetFlag(SF_MESREAD);
  SetFlag(SF_MESSKIP, mesSkip);
  SetFlag(SF_MESALLSKIP, mesSkip);  // These two are seemingly identical?
}

}  // namespace Vm

}  // namespace Impacto