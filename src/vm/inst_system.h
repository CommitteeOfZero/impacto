#pragma once

#include "vm.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstDummy);

VmInstruction(InstEnd);
VmInstruction(InstCreateThread);
VmInstruction(InstKillThread);
VmInstruction(InstReset);
VmInstruction(InstScriptLoad);
VmInstruction(InstWait);
VmInstruction(InstHalt);
VmInstruction(InstGetLabelAdr);
VmInstruction(InstFlagOnWait);
VmInstruction(InstSetFlag);
VmInstruction(InstResetFlag);
VmInstruction(InstCopyFlag);
VmInstruction(InstKeyWait);
VmInstruction(InstKeyWaitTimer);
VmInstruction(InstMemberWrite);
VmInstruction(InstThreadControl);
VmInstruction(InstGetSelfPointer);
VmInstruction(InstVsync);
VmInstruction(InstTest);
VmInstruction(InstThreadControlStore);
VmInstruction(InstPadAct);
VmInstruction(InstCopyThreadWork);
VmInstruction(InstSave);
VmInstruction(InstSaveIconLoad);
VmInstruction(InstVoiceTableLoadMaybe);
VmInstruction(InstSetPadCustom);
VmInstruction(InstMwait);
VmInstruction(InstTerminate);
VmInstruction(InstDebugPrint);
VmInstruction(InstSystemMes);
VmInstruction(InstSystemMenu);
VmInstruction(InstGetNowTime);
VmInstruction(InstGetSystemStatus);
VmInstruction(InstReboot);
VmInstruction(InstReloadScript);
VmInstruction(InstReloadScriptMenu);
VmInstruction(InstDebugEditer);
VmInstruction(InstPadActEx);
VmInstruction(InstDebugSetup);
VmInstruction(InstGlobalSystemMessage);
VmInstruction(InstCalc);
VmInstruction(InstMSinit);
VmInstruction(InstSaveSlot);
VmInstruction(InstSystemMain);
VmInstruction(InstGameInfoInit);
VmInstruction(InstSystemDataReset);
VmInstruction(InstDebugData);
VmInstruction(InstAutoSave);

}  // namespace Vm

}  // namespace Impacto