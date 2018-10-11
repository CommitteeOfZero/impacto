#include "inst_system.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../game.h"
#include "../log.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstDummy) {}

VmInstruction(InstEnd) {
  StartInstruction;
  BlockThread;
  ResetInstruction;
}
VmInstruction(InstCreateThread) {
  StartInstruction;
  PopExpression(groupId);
  PopLocalLabel(labelAdr);
  Sc3VmThread* newThread = thread->VmContext->CreateThread(groupId);
  newThread->ScriptBufferId = groupId;
  newThread->Ip = labelAdr;
  thread->VmContext->RunThread(newThread);
  thread->VmContext->BlockCurrentScriptThread = false;
}
VmInstruction(InstKillThread) {
  StartInstruction;
  PopExpression(threadId);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction KillThread(threadId: %i)\n", threadId);
}
VmInstruction(InstReset) {
  StartInstruction;
  BlockThread;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Reset()\n");
}
VmInstruction(InstScriptLoad) {
  StartInstruction;
  PopExpression(bufferId);
  PopExpression(scriptId);
  thread->VmContext->LoadScript(bufferId, scriptId);
}
VmInstruction(InstWait) {
  StartInstruction;
  PopExpression(waitCycles);
  if (thread->WaitCounter <= 0) {
    thread->WaitCounter = waitCycles + 1;
  }
  thread->WaitCounter--;
  if (thread->WaitCounter > 0) {
    ResetInstruction;
    BlockThread;
  }
}
VmInstruction(InstHalt) {
  StartInstruction;
  thread->Flags |= TF_Pause;
  BlockThread;
  ResetInstruction;
}
VmInstruction(InstGetLabelAdr) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction GetLabelAdr(unknown)\n");
}
VmInstruction(InstFlagOnWait) {
  StartInstruction;
  PopUint8(checkVal);
  PopExpression(flagId);
  if (thread->GameContext->GetFlag(flagId) == (bool)checkVal) {
    ResetInstruction;
    BlockThread;
  }
}
VmInstruction(InstSetFlag) {
  StartInstruction;
  PopExpression(flagId);
  thread->GameContext->SetFlag(flagId, 1);
}
VmInstruction(InstResetFlag) {
  StartInstruction;
  PopExpression(flagId);
  thread->GameContext->SetFlag(flagId, 0);
}
VmInstruction(InstCopyFlag) {
  StartInstruction;
  PopExpression(flagIdSrc);
  PopExpression(flagIdDst);
  bool flagValSrc = thread->GameContext->GetFlag(flagIdSrc);
  thread->GameContext->SetFlag(flagIdDst, flagValSrc);
}
VmInstruction(InstKeyWait) {
  StartInstruction;
  PopUint8(type);
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction KeyWait(type: %i, arg1: %i, arg2: %i)\n", type,
             arg1, arg2);
}
VmInstruction(InstKeyWaitTimer) {
  StartInstruction;
  PopUint8(type);
  PopExpression(timer);
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction KeyWaitTimer(type: %i, timer: %i, arg1: %i, "
             "arg2: %i)\n",
             type, timer, arg1, arg2);
}
VmInstruction(InstMemberWrite) {
  StartInstruction;
  PopExpression(index);
  PopExpression(value);
  uint32_t* thdElement = (uint32_t*)thread->GetMemberPointer(index);
  *thdElement = value;
}
VmInstruction(InstThreadControl) {
  StartInstruction;
  PopExpression(groupId);
  PopExpression(controlType);
  thread->VmContext->ControlThreadGroup((ThreadGroupControlType)controlType,
                                        groupId);
}
VmInstruction(InstGetSelfPointer) {
  StartInstruction;
  thread->ScriptParam = thread->Id ^ 0x80000000;
}
VmInstruction(InstVsync) {
  StartInstruction;
  BlockThread;
}
VmInstruction(InstTest) {
  StartInstruction;
  PopExpression(testVal);
}
VmInstruction(InstThreadControlStore) {
  StartInstruction;
  PopUint8(type);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction ThreadControlStore(type: %i)\n", type);
}
VmInstruction(InstPadAct) {
  StartInstruction;
  PopExpression(unused);
  PopExpression(vib1);
  PopExpression(vib2);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction PadAct(unused: %i, vib1: %i, vib2: %i)\n",
             unused, vib1, vib2);
}
VmInstruction(InstCopyThreadWork) {
  StartInstruction;
  PopExpression(threadIdSrc);
  PopExpression(threadIdDst);
  PopExpression(beginIndex);
  PopExpression(count);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction CopyThreadWork(threadIdSrc: %i, threadIdDst: "
             "%i, beginIndex: %i, "
             "count: %i)\n",
             threadIdSrc, threadIdDst, beginIndex, count);
}
VmInstruction(InstSave) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 40:
      PopExpression(arg1);
      PopExpression(arg2);
      PopExpression(arg3);
      PopExpression(arg4);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction Save(type: %i, arg1: %i, arg2: %i, arg3: "
                 "%i, arg4: %i)\n",
                 type, arg1, arg2, arg3, arg4);
      break;
    default:
      ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction Save(type: %i)\n",
                 type);
      break;
  }
}
VmInstruction(InstSaveIconLoad) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SaveIconLoad(arg1: %i)\n",
             arg1);
}
VmInstruction(InstVoiceTableLoadMaybe) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction VoiceTableLoad(arg1: %i)\n", arg1);
}
VmInstruction(InstSetPadCustom) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SetPadCustom()\n");
}
VmInstruction(InstMwait) {
  StartInstruction;
  PopExpression(waitCycles);
  PopExpression(unused);
  if (thread->WaitCounter <= 0) {
    thread->WaitCounter = waitCycles + 1;
  }
  thread->WaitCounter--;
  // This wait is ignored if skip mode is enabled
  // TODO: Implement this when we have skip mode
  // if (SkipModeEnabled) {
  //  thread->WaitCounter = 0;
  //}
  if (thread->WaitCounter > 0) {
    ResetInstruction;
    BlockThread;
  }
}
VmInstruction(InstTerminate) {
  StartInstruction;
  BlockThread;
  ResetInstruction;
}
VmInstruction(InstDebugPrint) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LL_Debug, LC_VM, "DebugPrint(arg1: %i, arg2: %i)\n", arg1, arg2);
}
VmInstruction(InstSystemMes) {
  StartInstruction;
  PopUint8(mode);

  switch (mode) {
    case 0:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SystemMes(mode: SystemMesInit0)\n");
      break;
    case 1:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SystemMes(mode: SystemMesInit1)\n");
      break;
    case 2:
      PopExpression(sysMesInit2Arg);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SystemMes(mode: SystemMesInit2, "
                 "sysMesInit2Arg: %i)\n",
                 sysMesInit2Arg);
      break;
    case 3:
      PopUint16(sysMesStrNum);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SystemMes(mode: SystemMesSetMes, "
                 "sysMesStrNum: %i)\n",
                 sysMesStrNum);
      break;
    case 4:
      PopUint16(sysSelStrNum);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SystemMes(mode: SystemMesSetSel, "
                 "sysSelStrNum: %i)\n",
                 sysSelStrNum);
      break;
    case 5:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SystemMes(mode: SystemMesMain)\n");
      break;
    case 6:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SystemMes(mode: SystemMesFadeIn)\n");
      break;
    case 7:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SystemMes(mode: SystemMesFadeOut)\n");
      break;
    case 8:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SystemMes(mode: %i)\n", mode);
      break;
    case 20:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction SystemMes(mode: %i)\n", mode);
      break;
  }
}
VmInstruction(InstSystemMenu) {
  StartInstruction;
  PopUint8(mode);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SystemMenu(mode: %i)\n",
             mode);
}
VmInstruction(InstGetNowTime) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction GetNowTime(mode: SystemMesFadeIn)\n");
}
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
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction MSinit(initType: %i)\n",
             initType);
}
VmInstruction(InstSaveSlot) {}
VmInstruction(InstSystemMain) {}
VmInstruction(InstGameInfoInit) {}
VmInstruction(InstSystemDataReset) {}
VmInstruction(InstDebugData) {}
VmInstruction(InstAutoSave) {}

}  // namespace Vm

}  // namespace Impacto