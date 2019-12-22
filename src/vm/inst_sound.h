#pragma once

#include "vm.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstBGMplay);
VmInstruction(InstBGMstop);
VmInstruction(InstSEplay);
VmInstruction(InstSEplayMO6);
VmInstruction(InstSEstop);
VmInstruction(InstSSEplay);
VmInstruction(InstSSEstop);
VmInstruction(InstBGMflag);
VmInstruction(InstVoicePlay);
VmInstruction(InstVoicePlayOld);
VmInstruction(InstVoiceStop);
VmInstruction(InstVoiceStopNew);
VmInstruction(InstVoicePlayWait);
VmInstruction(InstBGMduelPlay);
VmInstruction(InstSNDpause);
VmInstruction(InstSEplayWait);
VmInstruction(InstResetSoundAll);
VmInstruction(InstSNDloadStop);
VmInstruction(InstBGMstopWait);
VmInstruction(InstSysVoicePlay);
VmInstruction(InstSysSeload);

}  // namespace Vm

}  // namespace Impacto