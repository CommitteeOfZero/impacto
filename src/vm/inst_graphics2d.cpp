#include "inst_graphics2d.h"

#include "expression.h"

#include "inst_macros.inc"

namespace Impacto {

namespace Vm {

VmInstruction(InstCreateSurf) {
  StartInstruction;
  PopUint8(type);
  PopExpression(surfaceId);
  PopExpression(width);
  PopExpression(height);
}
VmInstruction(InstReleaseSurf) {}
VmInstruction(InstLoadPic) {}
VmInstruction(InstSurfFill) {}
VmInstruction(InstSCcapture) {}
VmInstruction(InstBGload) {}
VmInstruction(InstBGswap) {}
VmInstruction(InstBGsetColor) {}
VmInstruction(InstBGsetLink) {}
VmInstruction(InstBGrelease) {}
VmInstruction(InstBGcopy) {}
VmInstruction(InstBGloadEx) {}
VmInstruction(InstGetCharaPause) {}
VmInstruction(InstBGfadeExpInit) {}
VmInstruction(InstBGeffectWave) {}
VmInstruction(InstBGeffect) {}

}  // namespace Vm

}  // namespace Impacto