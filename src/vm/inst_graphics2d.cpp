#include "inst_graphics2d.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../game.h"
#include "../log.h"
#include "../profile/scriptvars.h"
#include "../mem.h"
#include "../background2d.h"
#include "../character2d.h"
#include "interface/scene2d.h"
#include "../profile/vm.h"

namespace Impacto {

namespace Vm {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::Vm;

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
  if (surfaceId < 8) {
    if (Backgrounds2D[surfaceId].Status == LS_Loaded) {
      Backgrounds2D[surfaceId].Unload();
    }
  }
}
VmInstruction(InstLoadPic) {
  StartInstruction;
  PopExpression(surfaceId);
  PopExpression(archiveId);
  PopExpression(fileId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction LoadPic(surfaceId: %i, width: %i, height: %i)\n",
             surfaceId, archiveId, fileId);
  if (surfaceId < 8) {
    switch (archiveId) {
      case 0: {  // bg archive
        if (Backgrounds2D[surfaceId].Status == LS_Loading) {
          ResetInstruction;
          BlockThread;
        } else if (ScrWork[SW_BG1NO + ScrWorkBgStructSize * surfaceId] !=
                   fileId) {
          ScrWork[SW_BG1NO + ScrWorkBgStructSize * surfaceId] = fileId;
          Backgrounds2D[surfaceId].LoadAsync(fileId);
          ResetInstruction;
          BlockThread;
        }
      } break;
    }
  }
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
  int actualBufId = Interface::GetBufferId(bufferId);
  int bgBufId = ScrWork[SW_BG1SURF + actualBufId];
  if (Backgrounds2D[bgBufId].Status == LS_Loading) {
    ResetInstruction;
    BlockThread;
  } else if (ScrWork[SW_BG1NO + ScrWorkBgStructSize * actualBufId] !=
             backgroundId) {
    ScrWork[SW_BG1NO + ScrWorkBgStructSize * actualBufId] = backgroundId;
    Backgrounds2D[bgBufId].LoadAsync(backgroundId);
    ResetInstruction;
    BlockThread;
  }
}
VmInstruction(InstBGswap) {
  StartInstruction;
  PopExpression(srcBufferId);
  PopExpression(dstBufferId);

  srcBufferId = Interface::GetBufferId(srcBufferId);
  dstBufferId = Interface::GetBufferId(dstBufferId);

  bool bg1fl = GetFlag(SF_BG1DISP + srcBufferId);
  bool bg2fl = GetFlag(SF_BG1DISP + dstBufferId);
  SetFlag(SF_BG1DISP + srcBufferId, bg2fl);
  SetFlag(SF_BG1DISP + dstBufferId, bg1fl);

  int counter = 0;
  do {
    int temp =
        ScrWork[SW_BG1POSX + (srcBufferId * ScrWorkBgStructSize) + counter];
    ScrWork[SW_BG1POSX + (srcBufferId * ScrWorkBgStructSize) + counter] =
        ScrWork[SW_BG1POSX + (dstBufferId * ScrWorkBgStructSize) + counter];
    ScrWork[SW_BG1POSX + (dstBufferId * ScrWorkBgStructSize) + counter] = temp;
    counter++;
  } while (counter != ScrWorkBgStructSize);

  int tempb = ScrWork[SW_BG1SURF + srcBufferId];
  ScrWork[SW_BG1SURF + srcBufferId] = ScrWork[SW_BG1SURF + dstBufferId];
  ScrWork[SW_BG1SURF + dstBufferId] = tempb;
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
VmInstruction(InstBGsetLinkOld) {
  StartInstruction;
  PopUint8(target);
  PopExpression(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  int link = (arg3 << 16) + (arg1 << 8) + arg2;
  if (target)
    ScrWork[SW_BGLINK] = link;
  else
    ScrWork[SW_BGLINK2] = link;
}
VmInstruction(InstCHAload) {
  StartInstruction;
  PopUint8(arg1);
  PopExpression(bufferId);
  PopExpression(characterId);

  int actualBufId = Interface::GetBufferId(bufferId);
  int chaBufId = ScrWork[SW_CHA1SURF + actualBufId];
  if (((characterId & 0xFFFF0000) >> 16) == 0) {
    characterId =
        (ScrWork[SW_CHA1FACE + ScrWorkChaStructSize * actualBufId] << 16) |
        characterId;
  }
  if (Characters2D[chaBufId].Status == LS_Loading) {
    ResetInstruction;
    BlockThread;
  } else if (ScrWork[SW_CHA1NO + ScrWorkChaStructSize * actualBufId] !=
             (characterId & 0xFFFF)) {
    ScrWork[SW_CHA1NO + ScrWorkChaStructSize * actualBufId] =
        characterId & 0xFFFF;
    ScrWork[SW_CHA1FACE + ScrWorkChaStructSize * actualBufId] =
        characterId >> 16;
    Characters2D[chaBufId].LoadAsync(characterId);
    ResetInstruction;
    BlockThread;
  }
}
VmInstruction(InstCHAswap) {
  StartInstruction;
  PopExpression(srcBufferId);
  PopExpression(dstBufferId);

  srcBufferId = Interface::GetBufferId(srcBufferId);
  dstBufferId = Interface::GetBufferId(dstBufferId);

  bool cha1fl = GetFlag(SF_CHA1DISP + srcBufferId);
  bool cha2fl = GetFlag(SF_CHA1DISP + dstBufferId);
  SetFlag(SF_CHA1DISP + srcBufferId, cha2fl);
  SetFlag(SF_CHA1DISP + dstBufferId, cha1fl);

  int counter = 0;
  do {
    int temp =
        ScrWork[SW_CHA1POSX + (srcBufferId * ScrWorkChaStructSize) + counter];
    ScrWork[SW_CHA1POSX + (srcBufferId * ScrWorkChaStructSize) + counter] =
        ScrWork[SW_CHA1POSX + (dstBufferId * ScrWorkChaStructSize) + counter];
    ScrWork[SW_CHA1POSX + (dstBufferId * ScrWorkChaStructSize) + counter] =
        temp;
    counter++;
  } while (counter != ScrWorkChaStructSize);

  int tempb = ScrWork[SW_CHA1SURF + srcBufferId];
  ScrWork[SW_CHA1SURF + srcBufferId] = ScrWork[SW_CHA1SURF + dstBufferId];
  ScrWork[SW_CHA1SURF + dstBufferId] = tempb;
}
VmInstruction(InstBGrelease) {
  StartInstruction;
  PopExpression(bufferId);
  bufferId = Interface::GetBufferId(bufferId);
  int surfId = ScrWork[SW_BG1SURF + bufferId];
  if (Backgrounds2D[surfId].Status == LS_Loaded) {
    Backgrounds2D[surfId].Unload();
  }
}
VmInstruction(InstBGcopy) {
  StartInstruction;
  PopExpression(srcBufferId);
  PopExpression(dstBufferId);

  srcBufferId = Interface::GetBufferId(srcBufferId);
  dstBufferId = Interface::GetBufferId(dstBufferId);

  int bgId = ScrWork[SW_BG1NO + ScrWorkBgStructSize * srcBufferId];
  int dstSurfId = ScrWork[SW_BG1SURF + dstBufferId];

  if (Backgrounds2D[dstSurfId].Status == LS_Loading) {
    ResetInstruction;
    BlockThread;
  } else if (ScrWork[SW_BG1NO + ScrWorkBgStructSize * dstBufferId] != bgId) {
    ScrWork[SW_BG1NO + ScrWorkBgStructSize * dstBufferId] = bgId;
    Backgrounds2D[dstSurfId].LoadAsync(bgId);
    ResetInstruction;
    BlockThread;
  }
}
VmInstruction(InstCHAcopy) {
  StartInstruction;
  PopExpression(srcBufferId);
  PopExpression(dstBufferId);

  srcBufferId = Interface::GetBufferId(srcBufferId);
  dstBufferId = Interface::GetBufferId(dstBufferId);

  int chaId = (ScrWork[SW_CHA1FACE + ScrWorkBgStructSize * srcBufferId] << 16) +
              ScrWork[SW_CHA1NO + ScrWorkBgStructSize * srcBufferId];
  int dstSurfId = ScrWork[SW_CHA1SURF + dstBufferId];

  if (Characters2D[dstSurfId].Status == LS_Loading) {
    ResetInstruction;
    BlockThread;
  } else if (ScrWork[SW_CHA1NO + ScrWorkChaStructSize * dstBufferId] !=
             (chaId & 0xFFFF)) {
    ScrWork[SW_CHA1NO + ScrWorkChaStructSize * dstBufferId] = chaId & 0xFFFF;
    Characters2D[dstSurfId].LoadAsync(chaId);
    ResetInstruction;
    BlockThread;
  }
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
  bufferId = Interface::GetBufferId(bufferId);
  int surfId = ScrWork[SW_CHA1SURF + bufferId];
  if (Characters2D[surfId].Status == LS_Loaded) {
    Characters2D[surfId].Unload();
  }
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
  PopUint8(type);
  switch (type) {
    case 0:
    case 5: {
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
    } break;
    case 1:
    case 3:
    case 9: {
      PopExpression(arg1);
      PopExpression(arg2);
    } break;
    case 6:
    case 7: {
      PopExpression(arg1);
    } break;
  }
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction BGeffect(arg1: %i)\n",
             type);
}

VmInstruction(InstBGeffectMO7) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopExpression(arg1);
    } break;
  }
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction BGeffectMO7(arg1: %i)\n",
             type);
}

}  // namespace Vm

}  // namespace Impacto