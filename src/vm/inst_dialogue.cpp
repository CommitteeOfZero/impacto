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
VmInstruction(InstMesSetID) {}
VmInstruction(InstMesCls) {}
VmInstruction(InstMesVoiceWait) {}
VmInstruction(InstMes) {}
VmInstruction(InstMesMain) {}
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
VmInstruction(InstMessWindow) {}
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
VmInstruction(InstSetDic) {}
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