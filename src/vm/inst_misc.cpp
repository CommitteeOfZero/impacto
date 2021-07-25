#include "inst_misc.h"

#include "inst_macros.inc"

#include "expression.h"
#include "interface/input.h"
#include "../profile/scriptvars.h"
#include "../game.h"
#include "../mem.h"
#include "../log.h"
#include "../hud/saveicondisplay.h"
#include "../ui/ui.h"
#include "../inputsystem.h"
#include "../savesystem.h"

#include "../profile/vm.h"

namespace Impacto {

namespace Vm {

using namespace Impacto::Profile::ScriptVars;

VmInstruction(InstUPLmenuUI) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction UPLmenuUI(arg1: %i)\n",
             arg1);
}
VmInstruction(InstUPLxTitle) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction UPLxTitle(arg1: %i)\n",
             arg1);
}
VmInstruction(InstPresence) {
  StartInstruction;
  PopUint8(arg1);
  PopExpression(arg2);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction Presence(arg1: %i, arg2: %i)\n", arg1, arg2);
}
VmInstruction(InstPresenceMO6) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Presence(arg1: %i)\n",
             arg1);
}
VmInstruction(InstAchievement) {
  StartInstruction;
  PopUint8(type);
  if (type == 1) {
    PopExpression(arg1);
    ImpLogSlow(LL_Warning, LC_VMStub,
               "STUB instruction Achievement(type: %i, arg1: %i)\n", type,
               arg1);
  } else {
    ImpLogSlow(LL_Warning, LC_VMStub,
               "STUB instruction Achievement(type: %i)\n", type);
  }
}
VmInstruction(InstSetPlayer) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SetPlayer(arg1: %i)\n",
             arg1);
}
VmInstruction(InstSignIn) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SignIn()\n");
}
VmInstruction(InstAchievementIcon) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction AchievementIcon(arg1: %i)\n", arg1);
}
VmInstruction(InstSetX360SysMesPos) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction SetX360SysMesPos(arg1: %i)\n", arg1);
}
VmInstruction(InstSystemMenu) {
  StartInstruction;
  PopUint8(mode);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SystemMenu(mode: %i)\n",
             mode);
  switch (mode) {
    case 0:
      UI::SystemMenuPtr->Show();
      break;
    case 1: {
      if (!UI::SystemMenuPtr->ChoiceMade) {
        if (!(Interface::PADinputButtonWentDown & Interface::PAD1B) &&
            !(Interface::PADinputMouseWentDown & Interface::PAD1B)) {
          ResetInstruction;
          BlockThread;
        }
      } else {
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
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction PressStart(type: %i, arg1: %i, arg2: %i)\n",
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
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction PressStart(type: %i)\n", type);
      SaveIconDisplay::Hide();
    } break;
    case 2: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction PressStart(type: %i, arg1: %i, arg2: %i, "
                 "arg3: %i)\n",
                 type, arg1, arg2, arg3);
    } break;
    case 3: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction PressStart(type: %i, arg1: %i)\n", type,
                 arg1);
    } break;
    case 4: {
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction PressStart(type: %i)\n", type);
      SaveIconDisplay::Show();
    } break;
    case 5: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction PressStart(type: %i, arg1: %i, arg2: %i)\n",
                 type, arg1, arg2);
    } break;
    case 6: {
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction PressStart(type: %i)\n", type);
      SaveIconDisplay::Hide();
    } break;
    case 7: {
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction PressStart(type: %i)\n", type);
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
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction ClearFlagChk()\n");
}
VmInstruction(InstClearFlagChkOld) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction ClearFlagChk(arg1: %i)\n",
             arg1);
}
VmInstruction(InstOption) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:
    case 0xA:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Option(type: Init)\n");
      break;
    case 1:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Option(type: Main)\n");
      break;
    case 2:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Option(type: Cancel)\n");
      break;
    case 3:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Option(type: V2toV1vol)\n");
      break;
    case 4:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Option(type: Default)\n");
      break;
  }
}
VmInstruction(InstHelp) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // Init
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Help(type: Init)\n");
      break;
    case 1:  // Main
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Help(type: Main)\n");
      break;
  }
}
VmInstruction(InstAchievementMenu) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction AchievementMenu(arg1: %i)\n", arg1);
}
VmInstruction(InstSoundMenu) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // MusicInit
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SoundMenu(type: MusicInit)\n");
      break;
    case 1:  // MusicMain
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SoundMenu(type: MusicMain)\n");
      break;
    case 0xA:  // ProfSetXboxEvent
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SoundMenu(type: ProfSetXboxEvent)\n");
      break;
  }
}
VmInstruction(InstAllClear) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction AllClear()\n");
}
VmInstruction(InstAlbum) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // EXmenuInit
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Album(type: EXmenuInit)\n");
      break;
    case 1:  // EXmenuMain
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Album(type: EXmenuMain)\n");
      break;
    case 0xA:  // ProfSetXboxEvent
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Album(type: ProfSetXboxEvent)\n");
      break;
  }
}
VmInstruction(InstMovieMode) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // Init
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MovieMode(type: Init)\n");
      break;
    case 1:  // Main
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MovieMode(type: Main)\n");
      break;
  }
}
VmInstruction(InstClistInit) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // PDmenuInit
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction ClistInit(type: EXmenuInit)\n");
      break;
    case 1:  // PlayDataMain
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction ClistInit(type: EXmenuMain)\n");
      break;
    case 3:  // Unused
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction ClistInit(type: %i)\n", type);
      break;
    case 0xA:  // ProfSetXboxEvent
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction ClistInit(type: ProfSetXboxEvent)\n");
      break;
  }
}
VmInstruction(InstSaveMenu) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // SaveMenuInit
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SaveMenu(type: SaveMenuInit)\n");
      break;
    case 1:  // SaveMenuMain
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SaveMenu(type: SaveMenuMain)\n");
      break;
    case 2:  // SaveResetThumnail
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SaveMenu(type: SaveResetThumnail)\n");
      break;
    case 10:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SaveMenu(type: %i)\n",
                 type);
      break;
    case 11:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SaveMenu(type: %i)\n",
                 type);
      break;
    case 12:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SaveMenu(type: %i)\n",
                 type);
      break;
    case 13:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SaveMenu(type: %i)\n",
                 type);
      break;
    case 20:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SaveMenu(type: %i)\n",
                 type);
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
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SaveMenu(type: %i, arg1: %i)\n", type, arg1);
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
        ScrWork[SW_SAVEFILESTATUS] = SaveSystem::GetSaveSatus(
            SaveSystem::SaveType::SaveFull, ScrWork[SW_SAVEFILENO]);
      }
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SaveMenu(type: %i)\n",
                 type);
      break;
    case 2: {
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SaveMenu(type: %i)\n",
                 type);
    } break;
    case 10: {
      PopUint8(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SaveMenu(type: %i, arg1: %i)\n", type, arg1);
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
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction LoadData(type: %i, arg1: %i)\n", type, arg1);
    } break;
    case 1:
    case 2:
    case 11:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction LoadData(type: %i)\n",
                 type);
      break;
  }
}
VmInstruction(InstLoadDataOld) {
  StartInstruction;
  PopExpression(arg1);
  SaveSystem::LoadMemory(SaveSystem::SaveFull, arg1);
  if (ScrWork[SW_MESWINDOW_COLOR] == 0) ScrWork[SW_MESWINDOW_COLOR] = 0xFFFFFF;
}
VmInstruction(InstTitleMenu) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // Init
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction TitleMenu(type: Init)\n");
      SetFlag(SF_TITLEMODE, 1);
      break;
    case 1:  // Main
      // Hack to kickstart into "New Game"
      switch (Profile::Vm::GameInstructionSet) {
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
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction TitleMenu(type: Main)\n");
      break;
    case 2:  // Init2
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction TitleMenu(type: Init2)\n");
      break;
      break;
  }
}
VmInstruction(InstTitleMenuNew) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // Init
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction TitleMenu(type: Init)\n");
      break;
    case 1:  // Main
      // Hack to kickstart into "New Game"
      if (ScrWork[SW_TITLEMODE] == 3) {
        if (((Interface::PADinputButtonWentDown & Interface::PAD1A) ||
             (Interface::PADinputMouseWentDown & Interface::PAD1A))) {
          ScrWork[2139] = 0;
          SetFlag(1241, 1);
        }
      } else if (ScrWork[SW_TITLEMODE] == 1 && ScrWork[SW_TITLEDISPCT] == 60) {
        // Check "PRESS TO START" here
        if (((Interface::PADinputButtonWentDown & Interface::PAD1A) ||
             (Interface::PADinputMouseWentDown & Interface::PAD1A))) {
          ScrWork[SW_TITLEMODE] = 2;
          ScrWork[SW_TITLEDISPCT] = 0;
          ScrWork[SW_TITLEMOVIECT] = 0;
          SetFlag(1241, 1);
        }
      }
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction TitleMenu(type: Main)\n");
      break;
    case 2:  // Init2
      ImpLogSlow(LL_Warning, LC_VMStub,
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
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SetPlayMode(arg1: %i)\n",
             arg1);
}
VmInstruction(InstSetEVflag) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SetEVflag(arg1: %i)\n",
             arg1);
}
VmInstruction(InstSetCutin) {
  StartInstruction;
  PopUint8(type);
  PopExpression(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  ImpLogSlow(
      LL_Warning, LC_VMStub,
      "STUB instruction SetCutin(type: %i, arg1: %i, arg2: %i, arg3: %i)\n",
      type, arg1, arg2, arg3);
}
VmInstruction(InstAchChkTitle) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction AchChkTitle()\n");
}
VmInstruction(InstSetSceneViewFlag) {
  StartInstruction;
  PopExpression(sceneId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction SetSceneViewFlag(sceneId: %i)\n", sceneId);
}
VmInstruction(InstChkClearFlag) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction ChkClearFlag()\n");
}

}  // namespace Vm

}  // namespace Impacto