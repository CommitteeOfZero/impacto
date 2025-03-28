#include "inst_misc.h"

#include "inst_macros.inc"

#include "expression.h"
#include "interface/input.h"
#include "../profile/scriptvars.h"
#include "../profile/configsystem.h"
#include "../game.h"
#include "../mem.h"
#include "../log.h"
#include "../hud/saveicondisplay.h"
#include "../ui/ui.h"
#include "../data/savesystem.h"

#include "../profile/vm.h"
#include "../games/cclcc/systemmenu.h"
namespace Impacto {

namespace Vm {

using namespace Impacto::Profile::ScriptVars;

VmInstruction(InstUPLmenuUI) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction UPLmenuUI(arg1: {:d})\n", arg1);
}
VmInstruction(InstUPLxTitle) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction UPLxTitle(arg1: {:d})\n", arg1);
}
VmInstruction(InstPresence) {
  StartInstruction;
  PopUint8(arg1);
  PopExpression(arg2);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Presence(arg1: {:d}, arg2: {:d})\n", arg1, arg2);
}
VmInstruction(InstPresenceMO6) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction Presence(arg1: {:d})\n", arg1);
}
VmInstruction(InstSetAchievement) {
  StartInstruction;
  PopUint8(type);
  if (type == 1) {
    PopExpression(arg1);
    ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
               "STUB instruction Achievement(type: {:d}, arg1: {:d})\n", type,
               arg1);
  } else {
    ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
               "STUB instruction Achievement(type: {:d})\n", type);
  }
}
VmInstruction(InstSetPlayer) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SetPlayer(arg1: {:d})\n", arg1);
}
VmInstruction(InstSignIn) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SignIn()\n");
}
VmInstruction(InstAchievementIcon) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction AchievementIcon(arg1: {:d})\n", arg1);
}
VmInstruction(InstSetX360SysMesPos) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SetX360SysMesPos(arg1: {:d})\n", arg1);
}
VmInstruction(InstSystemMenu) {
  StartInstruction;
  PopUint8(mode);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SystemMenu(mode: {:d})\n", mode);
  switch (mode) {
    case 0:
      if (Profile::Vm::GameInstructionSet == +InstructionSet::MO6TW) {
      } else if (Profile::Vm::GameInstructionSet == +InstructionSet::CC) {
        auto* sysMenuPtr =
            static_cast<UI::CCLCC::SystemMenu*>(UI::SystemMenuPtr);
        sysMenuPtr->Init();
        // Block input during animation
        if (sysMenuPtr->State == UI::MenuState::Hiding ||
            sysMenuPtr->State == UI::MenuState::Showing) {
          ResetInstruction;
          BlockThread;
        }
      }

      break;
    case 1: {
      if (UI::SystemMenuPtr->ChoiceMade) {
        UI::SystemMenuPtr->ChoiceMade = false;
        Interface::PADinputButtonWentDown |= Interface::PAD1A;
      }
    } break;
  }
}
VmInstruction(InstPressStart) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction PressStart(type: {:d}, arg1: {:d}, arg2: {:d})\n",
          type, arg1, arg2);
      switch (Profile::Vm::GameInstructionSet) {
        case InstructionSet::Dash: {
          SaveIconDisplay::ShowAt(glm::vec2(arg1 * 1.5f, arg2 * 1.5f));
        } break;
        default: {
          SaveIconDisplay::ShowAt(glm::vec2(arg1, arg2));
        } break;
      }
    } break;
    case 1: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction PressStart(type: {:d})\n", type);
      SaveIconDisplay::Hide();
    } break;
    case 2: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction PressStart(type: {:d}, arg1: {:d}, arg2: {:d}, "
          "arg3: {:d})\n",
          type, arg1, arg2, arg3);
    } break;
    case 3: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction PressStart(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 4: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction PressStart(type: {:d})\n", type);
      SaveIconDisplay::Show();
    } break;
    case 5: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction PressStart(type: {:d}, arg1: {:d}, arg2: {:d})\n",
          type, arg1, arg2);
    } break;
    case 6: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction PressStart(type: {:d})\n", type);
      SaveIconDisplay::Hide();
    } break;
    case 7: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction PressStart(type: {:d})\n", type);
      SaveIconDisplay::Hide();
    } break;
  }
}
VmInstruction(InstPressStartNew) {
  StartInstruction;
  PopUint8(type);
  if (type == 0 || type == 3) {
    PopLocalLabel(labelAdr);
    thread->Ip = labelAdr;
  }
}
VmInstruction(InstClearFlagChk) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction ClearFlagChk()\n");
}
VmInstruction(InstEVinit) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction ClearFlagInit(arg1: {:d})\n", arg1);
}
VmInstruction(InstEVload) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction ClearFlagLoad(arg1: {:d})\n", arg1);
}
VmInstruction(InstEVset) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction ClearFlagSet(arg1: {:d})\n", arg1);
}
VmInstruction(InstClearFlagChkOld) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction ClearFlagChk(arg1: {:d})\n", arg1);
}
VmInstruction(InstOption) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:
    case 0xA:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Option(type: Init)\n");
      break;
    case 1:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Option(type: Main)\n");
      break;
    case 2:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Option(type: Cancel)\n");
      break;
    case 3:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Option(type: V2toV1vol)\n");
      break;
    case 4:
      UI::OptionsMenuPtr->ResetToDefault();
      break;
  }
}
VmInstruction(InstHelp) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // Init
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Help(type: Init)\n");
      break;
    case 1:  // Main
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Help(type: Main)\n");
      break;
  }
}
VmInstruction(InstAchievementMenu) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // Init
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction AchievementMenu(type: Init)\n");
      break;
    case 1:  // Main
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction AchievementMenu(type: Main)\n");
      break;
    case 0xA:  // ProfSetXboxEvent
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction AchievementMenu(type: ProfSetXboxEvent)\n");
      break;
  }
}
VmInstruction(InstSoundMenu) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // MusicInit
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SoundMenu(type: MusicInit)\n");
      break;
    case 1:  // MusicMain
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SoundMenu(type: MusicMain)\n");
      break;
    case 0xA:  // ProfSetXboxEvent
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SoundMenu(type: ProfSetXboxEvent)\n");
      break;
  }
}
VmInstruction(InstAllClear) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction AllClear()\n");
}
VmInstruction(InstAlbum) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // EXmenuInit
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Album(type: EXmenuInit)\n");
      break;
    case 1:  // EXmenuMain
      if (!((Interface::PADinputButtonWentDown & Interface::PAD1B) ||
            (Interface::PADinputMouseWentDown & Interface::PAD1B))) {
        ResetInstruction;
        BlockThread;
      } else {
        SetFlag(SF_ALBUMEND, true);
        Interface::PADinputButtonWentDown |= Interface::PAD1A;
      }
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Album(type: EXmenuMain)\n");
      break;
    case 0xA:  // ProfSetXboxEvent
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Album(type: ProfSetXboxEvent)\n");
      break;
  }
}
VmInstruction(InstMovieMode) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // Init
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MovieMode(type: Init)\n");
      break;
    case 1:  // Main
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MovieMode(type: Main)\n");
      break;
  }
}
VmInstruction(InstClistInit) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // PDmenuInit
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction ClistInit(type: EXmenuInit)\n");
      break;
    case 1:  // PlayDataMain
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction ClistInit(type: EXmenuMain)\n");
      break;
    case 3:  // Unused
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction ClistInit(type: {:d})\n", type);
      break;
    case 0xA:  // ProfSetXboxEvent
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction ClistInit(type: ProfSetXboxEvent)\n");
      break;
  }
}
VmInstruction(InstSaveMenu) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // SaveMenuInit
      PopUint8(arg1);
      ScrWork[SW_SAVEMENUMODE] = arg1;
      ScrWork[SW_SAVEFILESTATUS] = 0;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SaveMenu(type: SaveMenuInit)\n");
    } break;
    case 1:  // SaveMenuMain
      if (!UI::SaveMenuPtr->ChoiceMade) {
        if (!((Interface::PADinputButtonWentDown & Interface::PAD1B) ||
              (Interface::PADinputMouseWentDown & Interface::PAD1B))) {
          ResetInstruction;
          BlockThread;
        }
      } else {
        UI::SaveMenuPtr->ChoiceMade = false;
        Interface::PADinputButtonWentDown |= Interface::PAD1A;
      }
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SaveMenu(type: SaveMenuMain)\n");
      break;
    case 2:  // SaveResetThumnail
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SaveMenu(type: SaveResetThumnail)\n");
      break;
    case 10:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SaveMenu(type: {:d})\n", type);
      break;
    case 11:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SaveMenu(type: {:d})\n", type);
      break;
    case 12:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SaveMenu(type: {:d})\n", type);
      break;
    case 13:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SaveMenu(type: {:d})\n", type);
      break;
    case 20:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SaveMenu(type: {:d})\n", type);
      break;
  }
}
VmInstruction(InstSaveMenuOld) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopUint8(arg1);
      ScrWork[SW_SAVEMENUMODE] = arg1;
      ScrWork[SW_SAVEFILESTATUS] = 0;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SaveMenu(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 1:
      if (!UI::SaveMenuPtr->ChoiceMade) {
        if (!((Interface::PADinputButtonWentDown & Interface::PAD1B) ||
              (Interface::PADinputMouseWentDown & Interface::PAD1B))) {
          ResetInstruction;
          BlockThread;
        }
      } else {
        UI::SaveMenuPtr->ChoiceMade = false;
        Interface::PADinputButtonWentDown |= Interface::PAD1A;
        ScrWork[SW_SAVEFILESTATUS] = SaveSystem::GetSaveStatus(
            SaveSystem::SaveType::SaveFull, ScrWork[SW_SAVEFILENO]);
      }
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SaveMenu(type: {:d})\n", type);
      break;
    case 2: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SaveMenu(type: {:d})\n", type);
    } break;
    case 10: {
      PopUint8(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SaveMenu(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
  }
}
VmInstruction(InstLoadData) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:
    case 10: {
      PopExpression(arg1);
      PopExpression(arg2);
      SaveSystem::LoadEntry(static_cast<SaveSystem::SaveType>(arg1), arg2);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction LoadData(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 1:
    case 2:
    case 11:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction LoadData(type: {:d})\n", type);
      break;
  }
  SaveSystem::LoadMemoryNew(static_cast<SaveSystem::LoadProcess>(type));
}
VmInstruction(InstLoadDataOld) {
  StartInstruction;
  PopExpression(arg1);
  SaveSystem::LoadEntry(SaveSystem::SaveFull, arg1);
  if (ScrWork[SW_MESWINDOW_COLOR] == 0) ScrWork[SW_MESWINDOW_COLOR] = 0xFFFFFF;
}
VmInstruction(InstTitleMenu) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // Init
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction TitleMenu(type: Init)\n");
      SetFlag(SF_TITLEMODE, 1);
      break;
    case 1:  // Main
      // Hack to kickstart into "New Game"
      switch (Profile::Vm::GameInstructionSet) {
        default:
          break;
        case InstructionSet::RNE:
        case InstructionSet::MO7:
          ScrWork[SW_TITLECUR1] = 0;
          break;
        case InstructionSet::Dash:
          ScrWork[SW_TITLECUR1] = 2;
          break;
      }
      ScrWork[SW_TITLECUR2] = 255;
      SetFlag(SF_TITLEMODE, 0);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction TitleMenu(type: Main)\n");
      break;
    case 2:  // Init2
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction TitleMenu(type: Init2)\n");
      break;
  }
}
VmInstruction(InstTitleMenuNew) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // Init
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction TitleMenu(type: Init)\n");
      break;
    case 1:  // Main
      switch (Profile::Vm::GameInstructionSet) {
        default:
          break;
        case InstructionSet::CC:
        case InstructionSet::CHN: {
          if (ScrWork[SW_TITLEMODE] == 3) {
            if (!UI::TitleMenuPtr->AllowsScriptInput) {
              ResetInstruction;
              BlockThread;
            }
          } else if (ScrWork[SW_TITLEMODE] == 1 &&
                     ScrWork[SW_TITLEDISPCT] ==
                         (Profile::Vm::GameInstructionSet == +InstructionSet::CC
                              ? 60
                              : 400)) {
            // Check "PRESS TO START" here
            if (((Interface::PADinputButtonWentDown & Interface::PAD1A) ||
                 (Interface::PADinputMouseWentDown & Interface::PAD1A))) {
              ScrWork[SW_TITLEMODE] = 2;
              ScrWork[SW_TITLEDISPCT] = 0;
              ScrWork[SW_TITLEMOVIECT] = 0;
              SetFlag(SF_TITLEEND, 1);
            }
          }
        } break;
        case InstructionSet::MO8: {
          if (ScrWork[SW_TITLEMODE] == 1) {
            ScrWork[SW_TITLEMOVIECT] += 1;
            // Check "PRESS TO START" here
            if (((Interface::PADinputButtonWentDown & Interface::PAD1A) ||
                 (Interface::PADinputMouseWentDown & Interface::PAD1A))) {
              Audio::Channels[Audio::AC_SSE]->Play("sysse", 0, false, 0.0f);
              ScrWork[SW_TITLEMODE] = 2;
              ScrWork[SW_TITLEDISPCT] = 0;
              ScrWork[SW_TITLEMOVIECT] = 0;
              SetFlag(SF_TITLEEND, 1);
            }
          }
        } break;
      }
      break;
    case 2:  // Init2
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction TitleMenu(type: Init2)\n");
      break;
      break;
  }
}
VmInstruction(InstTitleMenuOld) {
  StartInstruction;
  // Sometimes in order to make something sane
  // you have to sacrifice the sanity of others...
  if (!UI::TitleMenuPtr->ChoiceMade) {
    if (!((Interface::PADinputButtonWentDown & Interface::PAD1B) ||
          (Interface::PADinputMouseWentDown & Interface::PAD1B))) {
      ResetInstruction;
      BlockThread;
    }
  } else {
    UI::TitleMenuPtr->ChoiceMade = false;
    Interface::PADinputButtonWentDown |= Interface::PAD1A;
  }

  // if (TitleMenu::Implementation != 0) {
  //  // ScrWork[SW_TITLECUR1] = TitleMenu::Implementation->CurrentChoice;
  //  ScrWork[SW_TITLECUR2] = TitleMenu::Implementation->SecondaryChoice;
  //} else {
  //  // ScrWork[SW_TITLECUR1] = 0;
  //  ScrWork[SW_TITLECUR2] = 255;
  //}
}
VmInstruction(InstSetPlayMode) {
  StartInstruction;
  PopExpression(arg1);
  MesSkipMode = (arg1 == 4) ? (!Profile::ConfigSystem::SkipRead) + 1 : arg1;
}
VmInstruction(InstSetEVflag) {
  StartInstruction;
  if (Profile::Vm::GameInstructionSet == +InstructionSet::MO8 ||
      Profile::Vm::GameInstructionSet == +InstructionSet::CHN) {
    PopUint8(unk01);
  }
  PopExpression(arg1);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SetEVflag(arg1: {:d})\n", arg1);
}
VmInstruction(InstSetCutin) {
  StartInstruction;
  PopUint8(type);
  PopExpression(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SetCutin(type: {:d}, arg1: {:d}, arg2: {:d}, "
             "arg3: {:d})\n",
             type, arg1, arg2, arg3);
}
VmInstruction(InstAchChkTitle) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction AchChkTitle()\n");
}
VmInstruction(InstSetSceneViewFlag) {
  StartInstruction;
  PopExpression(sceneId);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SetSceneViewFlag(sceneId: {:d})\n", sceneId);
}
VmInstruction(InstChkClearFlag) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction ChkClearFlag()\n");
}
VmInstruction(InstScreenChange) { StartInstruction; }
VmInstruction(InstExitGame) { StartInstruction; }

}  // namespace Vm

}  // namespace Impacto