#include "inst_misc.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../scriptvars.h"
#include "../game.h"
#include "../mem.h"
#include "../log.h"
#include "../saveicondisplay.h"

namespace Impacto {

namespace Vm {

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
      SaveIconDisplay::PositionX = arg1;
      SaveIconDisplay::PositionY = arg2;
      SaveIconDisplay::AnimState = SaveIconDisplay::Showing;
    } break;
    case 1: {
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction PressStart(type: %i)\n", type);
      SaveIconDisplay::AnimState = SaveIconDisplay::Hiding;
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
      SaveIconDisplay::AnimState = SaveIconDisplay::Showing;
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
      SaveIconDisplay::AnimState = SaveIconDisplay::Hiding;
    } break;
    case 7: {
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction PressStart(type: %i)\n", type);
      SaveIconDisplay::AnimState = SaveIconDisplay::Hiding;
    } break;
  }
}
VmInstruction(InstClearFlagChk) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction ClearFlagChk()\n");
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
VmInstruction(InstTitleMenu) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // Init
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction TitleMenu(type: Init)\n");
      break;
    case 1:  // Main
      // Hack to kickstart into "New Game"
      ScrWork[SW_TITLECUR1] = 0;
      ScrWork[SW_TITLECUR2] = 255;
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