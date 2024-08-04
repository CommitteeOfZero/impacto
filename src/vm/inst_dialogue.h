#pragma once

#include "vm.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstMesViewFlag);
VmInstruction(InstSetMesWinPri);
VmInstruction(InstMesSync);
VmInstruction(InstMesSetID);
VmInstruction(InstMesCls);
VmInstruction(InstMesVoiceWait);
VmInstruction(InstMes);
VmInstruction(InstMesMain);
VmInstruction(InstSetMesModeFormat);
VmInstruction(InstSetNGmoji);
VmInstruction(InstMesRev);
VmInstruction(InstMessWindow);
VmInstruction(InstSel);
VmInstruction(InstSelect);
VmInstruction(InstSysSel);
VmInstruction(InstSysSelect);
VmInstruction(InstSetTextTable);
VmInstruction(InstSetDic);
VmInstruction(InstEncyclopedia);
VmInstruction(InstNameID);
VmInstruction(InstTips);
VmInstruction(InstSetRevMes);

void ChkMesSkip();

}  // namespace Vm

}  // namespace Impacto