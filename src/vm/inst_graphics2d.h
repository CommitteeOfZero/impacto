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
VmInstruction(InstBGsetLinkOld);
VmInstruction(InstCHAload);
VmInstruction(InstCHAswap);
VmInstruction(InstBGrelease);
VmInstruction(InstCHArelease);
VmInstruction(InstBGcopy);
VmInstruction(InstCHAcopy);
VmInstruction(InstCharaLayerLoad);
VmInstruction(InstCHAmove);
VmInstruction(InstBGloadEx);
VmInstruction(InstGetCharaPause);
VmInstruction(InstBGfadeExpInit);
VmInstruction(InstBGeffectWave);
VmInstruction(InstBGeffect);
VmInstruction(InstBGeffectMO7);

}  // namespace Vm

}  // namespace Impacto