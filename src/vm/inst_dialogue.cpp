#include "inst_dialogue.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../game.h"
#include "../log.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstMesViewFlag) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction MesViewFlag(type: Set, arg1: %i, arg2: %i)\n", arg1,
          arg2);
    } break;
    case 1: {
      PopExpression(dest);
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MesViewFlag(type: Check, dest: %i, arg1: "
                 "%i, arg2: %i)\n",
                 dest, arg1, arg2);
    } break;
  }
}
VmInstruction(InstSetMesWinPri) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  PopExpression(unused);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction MesViewFlag(arg1: %i, arg2: "
             "%i, unused: %i)\n",
             arg1, arg2, unused);
}
VmInstruction(InstMesSync) {}
VmInstruction(InstMesSetID) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopUint16(savePointId);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction MesSetID(type: SetSavePoint, savePointId: %i)\n",
          savePointId);
    } break;
    case 1: {
      PopUint16(savePointId);
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MesViewFlag(type: SetSavePoint1, "
                 "savePointId: %i, arg1: %i)\n",
                 savePointId, arg1);
    } break;
    case 2: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MesSetID(type: %i, arg1: %i)\n", type, arg1);
    } break;
  }
}
VmInstruction(InstMesCls) {}
VmInstruction(InstMesVoiceWait) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MesVoiceWait()\n");
}
VmInstruction(InstMes) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopExpression(characterId);
      PopString(line);
      uint8_t* oldIp = thread->Ip;
      thread->Ip = line;
      thread->GameContext->DialoguePages[thread->DialoguePageId].AddString(
          thread);
      thread->Ip = oldIp;
    } break;
    case 1: {
      PopExpression(audioId);
      PopExpression(characterId);
      PopString(line);
      uint8_t* oldIp = thread->Ip;
      thread->Ip = line;
      thread->GameContext->DialoguePages[thread->DialoguePageId].AddString(
          thread);
      thread->Ip = oldIp;
    } break;
    case 3: {
      PopExpression(audioId);
      PopExpression(animationId);
      PopExpression(characterId);
      PopString(line);
      uint8_t* oldIp = thread->Ip;
      thread->Ip = line;
      thread->GameContext->DialoguePages[thread->DialoguePageId].AddString(
          thread);
      thread->GameContext->DialoguePages[thread->DialoguePageId].AnimState =
          DPAS_Hidden;
      thread->Ip = oldIp;
    } break;
    case 0x0B: {
      PopExpression(audioId);
      PopExpression(animationId);
      PopExpression(characterId);
      PopString(line);
      uint8_t* oldIp = thread->Ip;
      thread->Ip = line;
      thread->GameContext->DialoguePages[thread->DialoguePageId].AddString(
          thread);
      thread->Ip = oldIp;
    } break;
  }
}
VmInstruction(InstMesMain) {
  StartInstruction;
  PopUint8(type);
  DialoguePage* currentPage =
      &thread->GameContext->DialoguePages[thread->DialoguePageId];
  if (type == 0) {
    switch (currentPage->AnimState) {
      case DPAS_Hidden:
        currentPage->Mode =
            (DialoguePageMode)
                thread->GameContext->ScrWork[4423];  // Only for page 0 for now
        currentPage->ADVBoxOpacity = 0.0f;
        currentPage->AnimState = DPAS_Showing;
        ResetInstruction;
        BlockThread;
        break;
      case DPAS_Shown: {
        // TODO: Check controls here
        thread->GameContext->DialoguePages[thread->DialoguePageId].AnimState =
            DPAS_Hiding;
      } break;
      default:
        ResetInstruction;
        BlockThread;
        break;
    }
  }
}

VmInstruction(InstSetMesModeFormat) {
  StartInstruction;
  PopExpression(id);
  PopLocalLabel(modeDataAdr);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction SetMesModeFormat(id: %i)\n", id);
}
VmInstruction(InstSetNGmoji) {
  StartInstruction;
  PopString(strAdr1);
  PopString(strAdr2);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SetNGmoji()\n");
}
VmInstruction(InstMesRev) {}
VmInstruction(InstMessWindow) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MessWindow(type: HideCurrent)\n");
      break;
    case 1:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MessWindow(type: ShowCurrent)\n");
      break;
    case 2:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MessWindow(type: AwaitShowCurrent)\n");
      break;
    case 3:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MessWindow(type: AwaitHideCurrent)\n");
      break;
    case 4:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MessWindow(type: Current04)\n");
      break;
    case 5: {
      PopExpression(messWindowId);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MessWindow(type: Hide, messWindowId: %i)\n",
                 messWindowId);
    } break;
    case 6: {
      PopExpression(messWindowId);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction MessWindow(type: HideSlow, messWindowId: %i)\n",
          messWindowId);
    } break;
    case 7: {
      PopExpression(messWindowId);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction MessWindow(type: HideSlow, messWindowId: %i)\n",
          messWindowId);
    } break;
  }
}
VmInstruction(InstSel) {}
VmInstruction(InstSelect) {}
VmInstruction(InstSysSel) {}
VmInstruction(InstSysSelect) {}
VmInstruction(InstSetTextTable) {
  StartInstruction;
  PopExpression(id);
  PopLocalLabel(tableDataAdr);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SetTextTable(id: %i)\n",
             id);
}
VmInstruction(InstSetDic) {
  StartInstruction;
  PopUint8(type);
  PopExpression(tipId);
  switch (type) {
    case 0:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SetTextTable(type: NewTip, tipId: %i)\n",
                 tipId);
      break;
    case 1: {
      PopExpression(flagId);
      ImpLogSlow(
          LL_Warning, LC_VMStub,
          "STUB instruction SetTextTable(type: Check, tipId: %i, flagId: %i)\n",
          tipId, flagId);
    } break;
    case 2:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SetTextTable(type: %i, tipId: %i)\n", type,
                 tipId);
      break;
  }
}
VmInstruction(InstNameID) {}
VmInstruction(InstTips) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopLocalLabel(tipsDataAdr);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Tips(type: TipsDataInit)\n");
    } break;
    case 1:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Tips(type: TipsInit)\n");
      break;
    case 2:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Tips(type: TipsMain)\n");
      break;
    case 3:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Tips(type: TipsEnd)\n");
      break;
    case 4:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Tips(type: TipsSet)\n");
      break;
    case 10:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Tips(type: Tips_ProfSetXboxEvent)\n");
      break;
  }
}

}  // namespace Vm

}  // namespace Impacto