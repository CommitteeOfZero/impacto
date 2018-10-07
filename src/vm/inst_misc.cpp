#include "inst_misc.h"

#include "expression.h"

#include "inst_macros.inc"

namespace Impacto {

namespace Vm {

VmInstruction(InstUPLmenuUI) {
  thread->Ip += 2;
  PopUint8(arg1);
}
VmInstruction(InstUPLxTitle) {}
VmInstruction(InstPresence) {
  thread->Ip += 2;
  PopUint8(arg1);
  PopExpression(arg2);
}
VmInstruction(InstAchievement) {}
VmInstruction(InstSetPlayer) {}
VmInstruction(InstSignIn) {}
VmInstruction(InstAchievementIcon) {}
VmInstruction(InstSetX360SysMesPos) {}
VmInstruction(InstPressStart) {}
VmInstruction(InstClearFlagChk) {}
VmInstruction(InstOption) {}
VmInstruction(InstHelp) {}
VmInstruction(InstAchievementMenu) {}
VmInstruction(InstSoundMenu) {}
VmInstruction(InstAllClear) {}
VmInstruction(InstAlbum) {}
VmInstruction(InstMovieMode) {}
VmInstruction(InstClistInit) {}
VmInstruction(InstSaveMenu) {}
VmInstruction(InstLoadData) {}
VmInstruction(InstTitleMenu) {}
VmInstruction(InstSetPlayMode) {}
VmInstruction(InstSetEVflag) {}
VmInstruction(InstSetCutin) {}
VmInstruction(InstAchChkTitle) {}
VmInstruction(InstSetSceneViewFlag) {}
VmInstruction(InstChkClearFlag) {}

}  // namespace Vm

}  // namespace Impacto