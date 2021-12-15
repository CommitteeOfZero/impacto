#include "inst_dialogue.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../profile/scriptvars.h"
#include "../profile/dialogue.h"
#include "../game.h"
#include "../mem.h"
#include "../inputsystem.h"
#include "../log.h"
#include "../audio/audiosystem.h"
#include "../audio/audiostream.h"
#include "../audio/audiochannel.h"
#include "../profile/vm.h"
#include "../hud/saveicondisplay.h"
#include "../data/savesystem.h"
#include "../data/tipssystem.h"
#include "../ui/ui.h"
#include "interface/input.h"

namespace Impacto {

namespace Vm {

using namespace Impacto::Profile::ScriptVars;

VmInstruction(InstMesViewFlag) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // Set
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction MesViewFlag(type: Set, arg1: %i, arg2: %i)\n", arg1,
          arg2);
    } break;
    case 1: {  // Check
      PopExpression(dest);
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MesViewFlag(type: Check, dest: %i, arg1: "
                 "%i, arg2: %i)\n",
                 dest, arg1, arg2);
    } break;
  }
}
VmInstruction(InstSetMesWinPri) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  PopExpression(unused);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction MesViewFlag(arg1: %i, arg2: "
             "%i, unused: %i)\n",
             arg1, arg2, unused);
}
VmInstruction(InstMesSync) {}
VmInstruction(InstMesSetID) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // SetSavePointPage0
      if (Profile::Vm::UseReturnIds) {
        PopUint16(savePointId);
        ImpLogSlow(
            LL_Warning, LC_VMStub,
            "STUB instruction MesSetID(type: SetSavePoint, savePointId: %i)\n",
            savePointId);
      } else {
        ImpLogSlow(LL_Warning, LC_VMStub,
                   "STUB instruction MesSetID(type: SetSavePoint)\n");
      }
    } break;
    case 1: {  // SetSavePointForPage
      PopUint16(savePointId);
      PopExpression(dialoguePageId);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MesSetID(type: SetSavePoint1, "
                 "savePointId: %i, arg1: %i)\n",
                 savePointId, dialoguePageId);
    } break;
    case 2: {  // SetPage
      PopExpression(dialoguePageId);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction MesSetID(type: SetPage, dialoguePageId: %i)\n",
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
    ImpLogSlow(LL_Warning, LC_VMStub,
               "STUB instruction MesCls(type: %i, arg1: %i)\n", type, arg1);
  } else {
    ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MesCls(type: %i)\n",
               type);
  }
}
VmInstruction(InstMesVoiceWait) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MesVoiceWait()\n");
}
VmInstruction(InstMes) {
  // Save before advancing Ip
  SaveSystem::SaveMemory();

  StartInstruction;

  // After loading a save we need to make sure the textbox is actually shown
  if (DialoguePages[thread->DialoguePageId].FadeAnimation.IsOut() &&
      GetFlag(thread->DialoguePageId + SF_MESWINDOW0OPENFL)) {
    DialoguePages[thread->DialoguePageId].Mode =
        (DialoguePageMode)ScrWork[SW_MESMODE0];
    DialoguePages[thread->DialoguePageId].FadeAnimation.StartIn(true);
  }

  PopUint8(type);
  switch (type) {
    case 0: {  // LoadDialogue
      PopExpression(characterId);
      PopString(line);
      uint8_t* oldIp = thread->Ip;
      thread->Ip = line;
      DialoguePages[thread->DialoguePageId].AddString(thread);
      thread->Ip = oldIp;
      UI::BacklogMenuPtr->AddMessage(line);
    } break;
    case 1: {  // LoadVoicedUnactedDialogue
      PopExpression(audioId);
      PopExpression(characterId);
      PopString(line);
      uint8_t* oldIp = thread->Ip;
      thread->Ip = line;
      DialoguePages[thread->DialoguePageId].AddString(thread);
      thread->Ip = oldIp;
    } break;
    case 3: {  // LoadVoicedDialogue
      PopExpression(audioId);
      PopExpression(animationId);
      PopExpression(characterId);
      PopString(line);
      Io::InputStream* stream;
      Io::VfsOpen("voice", audioId, &stream);
      uint8_t* oldIp = thread->Ip;
      thread->Ip = line;
      DialoguePages[thread->DialoguePageId].AddString(
          thread, Audio::AudioStream::Create(stream));
      thread->Ip = oldIp;
      UI::BacklogMenuPtr->AddMessage(line, audioId);
    } break;
    case 0x0B: {  // LoadVoicedDialogue0B
      PopExpression(audioId);
      PopExpression(animationId);
      PopExpression(characterId);
      PopString(line);
      uint8_t* oldIp = thread->Ip;
      thread->Ip = line;
      DialoguePages[thread->DialoguePageId].AddString(thread);
      thread->Ip = oldIp;
    } break;
    case 0x80: {  // LoadDialogueMSB
      PopExpression(characterId);
      PopMsbString(line);
      uint8_t* oldIp = thread->Ip;
      thread->Ip = line;
      DialoguePages[thread->DialoguePageId].AddString(thread);
      thread->Ip = oldIp;
    } break;
    case 0x83: {  // LoadVoicedDialogueMSB
      PopExpression(audioId);
      PopExpression(animationId);
      PopExpression(characterId);
      PopMsbString(line);
      Io::InputStream* stream;
      Io::VfsOpen("voice", audioId, &stream);
      uint8_t* oldIp = thread->Ip;
      thread->Ip = line;
      DialoguePages[thread->DialoguePageId].AddString(
          thread, Audio::AudioStream::Create(stream));
      thread->Ip = oldIp;
    } break;
  }
}
VmInstruction(InstMesMain) {
  StartInstruction;
  PopUint8(type);
  DialoguePage* currentPage = &DialoguePages[thread->DialoguePageId];
  if (type == 0) {  // Normal mode
    if ((Interface::PADinputButtonWentDown & Interface::PAD1A ||
         Interface::PADinputMouseWentDown & Interface::PAD1A) &&
        !currentPage->TextIsFullyOpaque() &&
        !currentPage->Typewriter.IsCancelled) {
      currentPage->Typewriter.CancelRequested = true;
      ResetInstruction;
    } else if (!((Interface::PADinputButtonWentDown & Interface::PAD1A ||
                  Interface::PADinputMouseWentDown & Interface::PAD1A) &&
                 currentPage->TextIsFullyOpaque())) {
      if (!GetFlag(SF_MESALLSKIP)) {
        ResetInstruction;
      }
    }
    BlockThread;
  }
  // TODO: Type 1 - Skip mode(?)
}
VmInstruction(InstSetMesModeFormat) {
  StartInstruction;
  PopExpression(id);
  PopLocalLabel(modeDataAdr);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction SetMesModeFormat(id: %i)\n", id);
}
VmInstruction(InstSetNGmoji) {
  StartInstruction;
  PopString(strAdr1);
  PopString(strAdr2);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SetNGmoji()\n");
}
VmInstruction(InstMesRev) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // DispInit
      UI::BacklogMenuPtr->Show();
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SetMesModeFormat(type: DispInit)\n");
      break;
    case 1:  // Main
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SetMesModeFormat(type: Main)\n");
      break;
    case 2:  // AllCls
      UI::BacklogMenuPtr->Hide();
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SetMesModeFormat(type: AllCls)\n");
      break;
    case 3:  // ChkLoad
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SetMesModeFormat(type: ChkLoad)\n");
      break;
    case 4:  // SAVELoad
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SetMesModeFormat(type: SAVELoad)\n");
      break;
    case 5:  // SoundUnk
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SetMesModeFormat(type: SoundUnk)\n");
      break;
    case 0xA:  // DispInit
      ImpLogSlow(LL_Warning, LC_VMStub,
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
        currentPage->Mode =
            (DialoguePageMode)ScrWork[SW_MESMODE0];  // Only for page 0 for now
        currentPage->FadeAnimation.StartIn(true);
        SetFlag(thread->DialoguePageId + SF_MESWINDOW0OPENFL, 1);
      }
      break;
    case 2:  // AwaitShowCurrent
      if (currentPage->FadeAnimation.State == AS_Playing) {
        ResetInstruction;
        BlockThread;
      }
      break;
    case 3:  // AwaitHideCurrent
      if (currentPage->FadeAnimation.State == AS_Playing) {
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
          LL_Warning, LC_VMStub,
          "STUB instruction MessWindow(type: HideSlow, messWindowId: %i)\n",
          messWindowId);
    } break;
    case 7: {  // HideSlow
      PopExpression(messWindowId);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction MessWindow(type: HideSlow, messWindowId: %i)\n",
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
        PopUint16(unused);
      }
      PopExpression(arg1);
      UI::SelectionMenuPtr->Init((bool)arg1);
      // SaveIconDisplay::Show();
      break;
    }
    case 1: {
      PopUint16(selStrNum);
      UI::SelectionMenuPtr->AddChoice(ScriptGetStrAddress(
          ScriptBuffers[thread->ScriptBufferId], selStrNum));
      break;
    }
    case 0x81: {
      PopMsbString(line);
      UI::SelectionMenuPtr->AddChoice(line);
    } break;
    case 2: {
      PopUint16(selStrNum);
      UI::SelectionMenuPtr->AddChoice(ScriptGetStrAddress(
          ScriptBuffers[thread->ScriptBufferId], selStrNum));
      PopExpression(arg2);
      break;
    }
    case 0x82: {
      PopMsbString(line);
      PopExpression(arg2);
      UI::SelectionMenuPtr->AddChoice(line);
    } break;
  }
}
VmInstruction(InstSelect) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      UI::SelectionMenuPtr->Show();
      SaveIconDisplay::ShowFor(2.4f);
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
  }
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SysSel(type: %i)\n",
             type);
}
VmInstruction(InstSysSelect) {
  StartInstruction;
  PopUint8(type);
  switch (type & 0xF) {
    case 0: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SysSelect(type: %i, arg1: %i, arg2: %i)\n",
                 type, arg1, arg2);
    } break;
    case 2:
    case 3: {
      PopExpression(destination);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SysSelect(type: %i, destination: %i)\n",
                 type, destination);
    } break;
  }
}
VmInstruction(InstSetTextTable) {
  StartInstruction;
  PopExpression(id);
  PopLocalLabel(tableDataAdr);
  TextTable[id].scriptBufferAdr = ScriptBuffers[thread->ScriptBufferId];
  TextTable[id].labelAdr = tableDataAdr;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SetTextTable(id: %i)\n",
             id);
}
VmInstruction(InstSetDic) {
  StartInstruction;
  PopUint8(type);
  PopExpression(tipId);
  switch (type) {
    case 0:  // NewTip
      TipsSystem::SetTipLockedState(tipId, false);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SetDic(type: NewTip, tipId: %i)\n", tipId);
      break;
    case 1: {  // Check
      PopExpression(flagId);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction SetDic(type: Check, tipId: %i, flagId: %i)\n",
          tipId, flagId);
    } break;
    case 2:  // SetDic02
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SetDic(type: %i, tipId: %i)\n", type, tipId);
      break;
  }
}
VmInstruction(InstEncyclopedia) {
  StartInstruction;
  PopExpression(tipId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction Encyclopedia(tipId: %i)\n", tipId);
  TipsSystem::SetTipLockedState(tipId, false);
}
VmInstruction(InstNameID) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:
      if (Profile::Vm::GameInstructionSet == +InstructionSet::CC ||
          Profile::Vm::GameInstructionSet == +InstructionSet::MO8) {
        PopLocalLabel(namePlateDataBlock);
      } else if (Profile::Vm::GameInstructionSet == +InstructionSet::MO6TW) {
        PopExpression(arg1);
        PopExpression(arg2);
        PopExpression(arg3);
      }
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction NameID(type: %i)\n",
                 type);
      break;
    case 1: {
      PopLocalLabel(namePlateDataBlock);
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction NameID(type: %i)\n",
                 type);
    } break;
    case 2:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction NameID(type: %i)\n",
                 type);
      break;
  }
}
VmInstruction(InstTips) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // TipsDataInit
      PopLocalLabel(tipsDataAdr);
      if (Profile::Vm::GameInstructionSet == +InstructionSet::MO8) {
        PopLocalLabel(tipsDataAdr1);
      }

      TipsSystem::DataInit(thread->ScriptBufferId, tipsDataAdr);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Tips(type: TipsDataInit)\n");
    } break;
    case 1:  // TipsInit
      TipsSystem::UpdateTipRecords();
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Tips(type: TipsInit)\n");
      break;
    case 2:  // TipsMain
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Tips(type: TipsMain)\n");
      break;
    case 3:  // TipsEnd
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Tips(type: TipsEnd)\n");
      break;
    case 4:  // TipsSet
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Tips(type: TipsSet)\n");
      break;
    case 10:  // Tips_ProfSetXboxEvent
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Tips(type: Tips_ProfSetXboxEvent)\n");
      break;
  }
}
VmInstruction(InstSetRevMes) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopString(message);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SetRevMes(type: %i)\n", type);
    } break;
    case 0x80: {
      PopExpression(messageId);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SetRevMes(type: %i)\n", type);
    } break;
    case 1: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopString(message);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SetRevMes(type: %i, arg1: %i, arg2: %i)\n",
                 type, arg1, arg2);
    } break;
    case 2: {
      PopString(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SetRevMes(type: %i)\n", type);
    } break;
    case 3: {
      PopString(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SetRevMes(type: %i, arg2: %i, arg3: %i, "
                 "arg4: %i)\n",
                 type, arg2, arg3, arg4);
    } break;
  }
}

}  // namespace Vm

}  // namespace Impacto