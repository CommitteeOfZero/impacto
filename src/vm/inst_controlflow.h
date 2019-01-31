#pragma once

#include "vm.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstJump);
VmInstruction(InstJumpTable);
VmInstruction(InstIf);
VmInstruction(InstCall);
VmInstruction(InstJumpFar);
VmInstruction(InstCallFar);
VmInstruction(InstReturn);
VmInstruction(InstReturnIfFlag);
VmInstruction(InstLoop);
VmInstruction(InstFlagOnJump);
VmInstruction(InstKeyOnJump);
VmInstruction(InstKeyOnJump_Dash);
VmInstruction(InstClickOnJump);
VmInstruction(InstLoadJump);
VmInstruction(InstSwitch);
VmInstruction(InstCase);

}  // namespace Vm

}  // namespace Impacto