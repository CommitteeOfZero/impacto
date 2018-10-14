#include "inst_misc.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../game.h"
#include "../log.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstUPLmenuUI) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction UPLmenuUI(arg1: %i)\n",
             arg1);
}
VmInstruction(InstUPLxTitle) {}
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
VmInstruction(InstAchievementIcon) {}
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
    } break;
    case 1: {
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction PressStart(type: %i)\n", type);
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
    } break;
    case 7: {
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction PressStart(type: %i)\n", type);
    } break;
  }
}
VmInstruction(InstClearFlagChk) {}
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
VmInstruction(InstHelp) {}
VmInstruction(InstAchievementMenu) {}
VmInstruction(InstSoundMenu) {}
VmInstruction(InstAllClear) {}
VmInstruction(InstAlbum) {}
VmInstruction(InstMovieMode) {}
VmInstruction(InstClistInit) {}
VmInstruction(InstSaveMenu) {}
VmInstruction(InstLoadData) {}
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
      thread->GameContext->ScrWork[3341] = 0;
      thread->GameContext->ScrWork[3342] = 255;
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
VmInstruction(InstSetPlayMode) {}
VmInstruction(InstSetEVflag) {}
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
VmInstruction(InstSetSceneViewFlag) {}
VmInstruction(InstChkClearFlag) {}

}  // namespace Vm

}  // namespace Impacto