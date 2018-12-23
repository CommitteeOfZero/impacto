#include "inst_graphics2d.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../game.h"
#include "../log.h"
#include "../scriptvars.h"
#include "../mem.h"
#include "../background2d.h"

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
  int bgBufId = ScrWork[SW_BG1SURF + (bufferId - 1)];
  if (Backgrounds2D[bgBufId].Status == LS_Loading) {
    ResetInstruction;
    BlockThread;
  } else if (ScrWork[SW_BG1NO + 40 * bufferId] != backgroundId) {
    ScrWork[SW_BG1NO + 40 * bufferId] = backgroundId;
    Backgrounds2D[bgBufId].LoadAsync(backgroundId);
    ResetInstruction;
    BlockThread;
  }
}
VmInstruction(InstBGswap) {
  StartInstruction;
  PopExpression(srcBufferId);
  PopExpression(dstBufferId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction BGswap(srcBufferId: %i, dstBufferId: %i)\n",
             srcBufferId, dstBufferId);

  bool bg1fl = GetFlag(SF_BG1DISP + (srcBufferId - 1));
  bool bg2fl = GetFlag(SF_BG1DISP + (dstBufferId - 1));
  SetFlag(SF_BG1DISP + (srcBufferId - 1), bg2fl);
  SetFlag(SF_BG1DISP + (dstBufferId - 1), bg1fl);

  int counter = 0;
  do {
    int temp = ScrWork[SW_BG1POSX + ((srcBufferId - 1) * 40) + counter];
    ScrWork[SW_BG1POSX + ((srcBufferId - 1) * 40) + counter] =
        ScrWork[SW_BG1POSX + ((dstBufferId - 1) * 40) + counter];
    ScrWork[SW_BG1POSX + ((dstBufferId - 1) * 40) + counter] = temp;
    counter++;
  } while (counter != 40);

  int tempb = ScrWork[SW_BG1SURF + (srcBufferId - 1)];
  ScrWork[SW_BG1SURF + (srcBufferId - 1)] =
      ScrWork[SW_BG1SURF + (dstBufferId - 1)];
  ScrWork[SW_BG1SURF + (dstBufferId - 1)] = tempb;
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
VmInstruction(InstCHAload) {
  StartInstruction;
  PopUint8(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction CHAload(arg1: %i, arg2: %i, arg3: %i)\n", arg1,
             arg2, arg3);
}
VmInstruction(InstCHAswap) {
  StartInstruction;
  PopExpression(srcBufferId);
  PopExpression(dstBufferId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction CHAswap(srcBufferId: %i, dstBufferId: %i)\n",
             srcBufferId, dstBufferId);
}
VmInstruction(InstBGrelease) {
  StartInstruction;
  PopExpression(bufferId);
  if (Backgrounds2D[bufferId - 1].Status == LS_Loaded) {
    Backgrounds2D[bufferId - 1].Unload();
  }
}
VmInstruction(InstBGcopy) {
  StartInstruction;
  PopExpression(srcBufferId);
  PopExpression(dstBufferId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction BGcopy(srcBufferId: %i, dstBufferId: %i)\n",
             srcBufferId, dstBufferId);
}
VmInstruction(InstCHAcopy) {
  StartInstruction;
  PopExpression(srcBufferId);
  PopExpression(dstBufferId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction CHAcopy(srcBufferId: %i, dstBufferId: %i)\n",
             srcBufferId, dstBufferId);
}
VmInstruction(InstCharaLayerLoad) { StartInstruction; }
VmInstruction(InstCHAmove) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction CHAmoveInit()\n");
      break;
    case 1: {
      PopExpression(arg1);
      PopLocalLabel(seqDataBlock);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction CHAmoveSetSeq(arg1: %i)\n", arg1);
    } break;
    case 2: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction CHAmoveExec_Blocking(arg1: %i)\n", arg1);
    } break;
    case 3:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction CHAmoveExec_NonBlocking()\n");
      break;
    case 4: {
      PopExpression(arg1);
      PopExpression(destination);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction CHAmoveChkEnd(arg1: %i, destination: %i)\n",
                 arg1, destination);
    } break;
    case 5: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      PopExpression(arg5);
      PopExpression(arg6);
      PopExpression(arg7);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction CHAmoveSetSeqDirect(arg1: %i, arg2: %i, "
                 "arg3: %i, arg4: %i, arg5: %i, arg6: %i, arg7: %i)\n",
                 arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    } break;
  }
}
VmInstruction(InstBGloadEx) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(backgroundId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction BGloadEx(bufferId: %i, backgroundId: %i)\n",
             bufferId, backgroundId);
}
VmInstruction(InstCHArelease) {
  StartInstruction;
  PopExpression(bufferId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction CHArelease(bufferId: %i)\n", bufferId);
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