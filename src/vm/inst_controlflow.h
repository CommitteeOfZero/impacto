#pragma once

#include "vm.h"
#include "inst_macros.inc"

namespace Impacto {

namespace Vm {

VmInstruction(InstJump);
VmInstruction(InstJumpTable);
VmInstruction(InstIf);
VmInstruction(InstCall);
VmInstruction(InstJumpFar);
VmInstruction(InstCallFar);
VmInstruction(InstReturn);
VmInstruction(InstLoop);
VmInstruction(InstFlagOnJump);
VmInstruction(InstKeyOnJump);
VmInstruction(InstLoadJump);
VmInstruction(InstSwitch);
VmInstruction(InstCase);

}  // namespace Vm

}  // namespace Impacto