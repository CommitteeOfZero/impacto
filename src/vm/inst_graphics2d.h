#pragma once

#include "vm.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstCreateSurf);
VmInstruction(InstReleaseSurf);
VmInstruction(InstLoadPic);
VmInstruction(InstSurfFill);
VmInstruction(InstSCcapture);
VmInstruction(InstBGload);
VmInstruction(InstBGswap);
VmInstruction(InstBGsetColor);
VmInstruction(InstBGsetLink);
VmInstruction(InstBGrelease);
VmInstruction(InstBGcopy);
VmInstruction(InstBGloadEx);
VmInstruction(InstGetCharaPause);
VmInstruction(InstBGfadeExpInit);
VmInstruction(InstBGeffectWave);
VmInstruction(InstBGeffect);

}  // namespace Vm

}  // namespace Impacto