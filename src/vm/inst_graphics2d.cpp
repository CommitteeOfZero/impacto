#include "inst_graphics2d.h"

#include "expression.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstCreateSurf) {
  thread->Ip += 2;
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