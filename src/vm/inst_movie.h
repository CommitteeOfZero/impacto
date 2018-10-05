#pragma once

#include "vm.h"
#include "inst_macros.inc"

namespace Impacto {

namespace Vm {

VmInstruction(InstPlayMovie);
VmInstruction(InstMovieMain);
VmInstruction(InstLoadMovie);
VmInstruction(InstSetRevMes);
VmInstruction(InstPlayMovieMemory);
VmInstruction(InstSFDpause);

}  // namespace Vm

}  // namespace Impacto