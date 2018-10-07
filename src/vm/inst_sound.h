#pragma once

#include "vm.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstBGMplay);
VmInstruction(InstBGMstop);
VmInstruction(InstSEplay);
VmInstruction(InstSEstop);
VmInstruction(InstSSEplay);
VmInstruction(InstSSEstop);
VmInstruction(InstBGMflag);
VmInstruction(InstVoicePlay);
VmInstruction(InstVoiceStop);
VmInstruction(InstVoicePlayWait);
VmInstruction(InstBGMduelPlay);
VmInstruction(InstSNDpause);
VmInstruction(InstSEplayWait);
VmInstruction(InstResetSoundAll);
VmInstruction(InstSNDloadStop);
VmInstruction(InstBGMstopWait);
VmInstruction(InstSysVoicePlay);

}  // namespace Vm

}  // namespace Impacto