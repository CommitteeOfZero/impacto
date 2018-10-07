#include "inst_system.h"

#include "expression.h"

#include "inst_macros.inc"

namespace Impacto {

namespace Vm {

VmInstruction(InstDummy) {}

VmInstruction(InstEnd) {}
VmInstruction(InstCreateThread) {}
VmInstruction(InstKillThread) {}
VmInstruction(InstReset) {}
VmInstruction(InstScriptLoad) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(scriptId);
  thread->VmContext->LoadScript(bufferId, scriptId);
}
VmInstruction(InstWait) {}
VmInstruction(InstHalt) {}
VmInstruction(InstGetLabelAdr) {}
VmInstruction(InstFlagOnWait) {}
VmInstruction(InstSetFlag) {}
VmInstruction(InstResetFlag) {}
VmInstruction(InstCopyFlag) {}
VmInstruction(InstKeyWait) {}
VmInstruction(InstKeyWaitTimer) {}
VmInstruction(InstMemberWrite) {}
VmInstruction(InstThreadControl) {}
VmInstruction(InstGetSelfPointer) {}
VmInstruction(InstVsync) {}
VmInstruction(InstTest) {}
VmInstruction(InstThreadControlStore) {}
VmInstruction(InstPadAct) {
  StartInstruction;
  PopExpression(unused);
  PopExpression(vib1);
  PopExpression(vib2);
}
VmInstruction(InstCopyThreadWork) {}
VmInstruction(InstSave) {}
VmInstruction(InstSaveIconLoad) {}
VmInstruction(InstVoiceTableLoadMaybe) {}
VmInstruction(InstSetPadCustom) {}
VmInstruction(InstMwait) {}
VmInstruction(InstTerminate) {}
VmInstruction(InstDebugPrint) {}
VmInstruction(InstSystemMes) {}
VmInstruction(InstSystemMenu) {}
VmInstruction(InstGetNowTime) {}
VmInstruction(InstGetSystemStatus) {}
VmInstruction(InstReboot) {}
VmInstruction(InstReloadScript) {}
VmInstruction(InstReloadScriptMenu) {}
VmInstruction(InstDebugEditer) {}
VmInstruction(InstPadActEx) {}
VmInstruction(InstDebugSetup) {}
VmInstruction(InstGlobalSystemMessage) {}
VmInstruction(InstCalc) {}
VmInstruction(InstMSinit) {
  StartInstruction;
  PopExpression(initType);
}
VmInstruction(InstSaveSlot) {}
VmInstruction(InstSystemMain) {}
VmInstruction(InstGameInfoInit) {}
VmInstruction(InstSystemDataReset) {}
VmInstruction(InstDebugData) {}
VmInstruction(InstAutoSave) {}

}  // namespace Vm

}  // namespace Impacto