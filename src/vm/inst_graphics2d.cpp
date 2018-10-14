#include "inst_graphics2d.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../game.h"
#include "../log.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstCreateSurf) {
  StartInstruction;
  PopUint8(type);
  PopExpression(surfaceId);
  PopExpression(width);
  PopExpression(height);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction CreateSurf(type: %i, surfaceId: "
             "%i, width: %i, "
             "height: %i)\n",
             type, surfaceId, width, height);
}
VmInstruction(InstReleaseSurf) {
  StartInstruction;
  PopExpression(surfaceId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction ReleaseSurf(surfaceId: %i)\n", surfaceId);
}
VmInstruction(InstLoadPic) {
  StartInstruction;
  PopExpression(surfaceId);
  PopExpression(archiveId);
  PopExpression(fileId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction LoadPic(surfaceId: %i, width: %i, height: %i)\n",
             surfaceId, archiveId, fileId);
}
VmInstruction(InstSurfFill) {
  StartInstruction;
  PopExpression(surfaceId);
  PopExpression(r);
  PopExpression(g);
  PopExpression(b);
  PopExpression(a);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction SurfFill(surfaceId: %i, r: "
             "%i, g: %i, "
             "b: %i, a: %i)\n",
             surfaceId, r, g, b, a);
}
VmInstruction(InstSCcapture) {
  StartInstruction;
  PopExpression(surfaceId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction SCcapture(surfaceId: %i)\n", surfaceId);
}
VmInstruction(InstBGload) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(backgroundId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction BGload(bufferId: %i, backgroundId: %i)\n",
             bufferId, backgroundId);
}
VmInstruction(InstBGswap) {
  StartInstruction;
  PopExpression(srcBufferId);
  PopExpression(dstBufferId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction BGswap(srcBufferId: %i, dstBufferId: %i)\n",
             srcBufferId, dstBufferId);
}
VmInstruction(InstBGsetColor) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(color);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction BGsetColor(bufferId: %i, color: %i)\n", bufferId,
             color);
}
VmInstruction(InstBGsetLink) {
  StartInstruction;
  PopUint8(id);
  PopExpression(arg1);
  PopExpression(arg2);
  if (id >= 4) {
    PopExpression(arg3);
  }
  PopExpression(arg4);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction BGsetLink(id: %i, arg1: "
             "%i, arg2: %i, "
             "arg4: %i)\n",
             id, arg1, arg2, arg4);
}
VmInstruction(InstBGrelease) {
  StartInstruction;
  PopExpression(bufferId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction BGrelease(bufferId: %i)\n", bufferId);
}
VmInstruction(InstBGcopy) {
  StartInstruction;
  PopExpression(srcBufferId);
  PopExpression(dstBufferId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction BGcopy(srcBufferId: %i, dstBufferId: %i)\n",
             srcBufferId, dstBufferId);
}
VmInstruction(InstBGloadEx) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(backgroundId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction BGloadEx(bufferId: %i, backgroundId: %i)\n",
             bufferId, backgroundId);
}
VmInstruction(InstGetCharaPause) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(dest);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction GetCharaPause(bufferId: %i, dest: %i)\n",
             bufferId, dest);
}
VmInstruction(InstBGfadeExpInit) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction BGfadeExpInit(arg1: %i)\n", arg1);
}
VmInstruction(InstBGeffectWave) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:
    case 2:  // Unimplemented
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction BGeffectWave(type: %i)\n", type);
      break;
    case 1: {  // BGwaveSetWave
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction BGeffectWave(type: BGwaveSetWave, arg1: %i, "
                 "arg2: %i, arg3: %i, arg4: %i, arg5: %i)\n",
                 arg1, arg2, arg3, arg4, arg5);
    } break;
    case 3: {  // CHAeffectWave
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction BGeffectWave(type: CHAeffectWave, arg1: %i, "
                 "arg2: %i, arg3: %i, arg4: %i, arg5: %i)\n",
                 arg1, arg2, arg3, arg4, arg5);
    } break;
    case 4: {  // BGwaveResetWave
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      PopExpression(arg6);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction BGeffectWave(type: BGwaveResetWave, arg1: %i, "
          "arg2: %i, arg3: %i, arg4: %i, arg5: %i, arg6: %i)\n",
          arg1, arg2, arg3, arg4, arg5, arg6);
    } break;
    case 5: {  // CHAwaveResetWave
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      PopExpression(arg6);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction BGeffectWave(type: CHAwaveResetWave, arg1: %i, "
          "arg2: %i, arg3: %i, arg4: %i, arg5: %i, arg6: %i)\n",
          arg1, arg2, arg3, arg4, arg5, arg6);
    } break;
    case 10:  // EFFwaveInitWave
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction BGeffectWave(type: EFFwaveInitWave)\n");
      break;
    case 11: {  // EFFwaveSetWave
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction BGeffectWave(type: EFFwaveSetWave, arg1: %i, "
          "arg2: %i, arg3: %i, arg4: %i, arg5: %i)\n",
          arg1, arg2, arg3, arg4, arg5);
    } break;
    case 12: {  // EFFwaveResetWave
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      PopExpression(arg6);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction BGeffectWave(type: EFFwaveResetWave, arg1: %i, "
          "arg2: %i, arg3: %i, arg4: %i, arg5: %i, arg6: %i)\n",
          arg1, arg2, arg3, arg4, arg5, arg6);
    } break;
  }
}
VmInstruction(InstBGeffect) {
  StartInstruction;
  PopUint8(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction BGeffect(arg1: %i)\n",
             arg1);
}

}  // namespace Vm

}  // namespace Impacto