#include "inst_controlflow.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../game.h"
#include "../log.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstJump) {
  StartInstruction;
  PopLocalLabel(labelAdr);

  thread->Ip = labelAdr;
}
VmInstruction(InstJumpTable) {
  StartInstruction;
  PopExpression(labelNumIndex);
  PopUint16(dataLabelNum);
  uint8_t* dataAdr = ScriptGetLabelAddress(
      thread->VmContext->ScriptBuffers[thread->ScriptBufferId], dataLabelNum);
  uint8_t* labelAdr = ScriptGetLabelAddress(
      thread->VmContext->ScriptBuffers[thread->ScriptBufferId],
      SDL_SwapLE16(*(uint16_t*)(dataAdr + 2 * labelNumIndex)));

  thread->Ip = labelAdr;
}
VmInstruction(InstIf) {
  StartInstruction;
  PopUint8(check);
  PopExpression(condition);
  PopLocalLabel(labelAdr);

  if ((bool)check == (bool)condition) {
    thread->Ip = labelAdr;
  }
}
VmInstruction(InstCall) {
  StartInstruction;
  PopLocalLabel(labelAdr);
  PopUint16(retNum);

  if (thread->CallStackDepth != VmMaxCallStackDepth) {
    thread->ReturnAdresses[thread->CallStackDepth] = retNum;
    thread->ReturnGroupIds[thread->CallStackDepth++] = thread->ScriptBufferId;
    thread->Ip = labelAdr;
  } else {
    ImpLog(LL_Error, LC_VM, "Call error, call stack overflow.\n");
  }
}
VmInstruction(InstJumpFar) {
  StartInstruction;
  PopExpression(scriptBufferId);
  PopFarLabel(labelAdr, scriptBufferId);

  thread->ScriptBufferId = scriptBufferId;
  thread->Ip = labelAdr;
}
VmInstruction(InstCallFar) {
  StartInstruction;
  PopExpression(scriptBufferId);
  PopFarLabel(labelAdr, scriptBufferId);
  PopUint16(retNum);

  if (thread->CallStackDepth != VmMaxCallStackDepth) {
    thread->ReturnAdresses[thread->CallStackDepth] = retNum;
    thread->ReturnGroupIds[thread->CallStackDepth++] = thread->ScriptBufferId;
    thread->Ip = labelAdr;
    thread->ScriptBufferId = scriptBufferId;
  } else {
    ImpLog(LL_Error, LC_VM, "CallFar error, call stack overflow.\n");
  }
}
VmInstruction(InstReturn) {
  StartInstruction;
  if (thread->CallStackDepth) {
    thread->CallStackDepth--;
    uint32_t retBufferId = thread->ReturnGroupIds[thread->CallStackDepth];
    thread->Ip =
        ScriptGetRetAddress(thread->VmContext->ScriptBuffers[retBufferId],
                            thread->ReturnAdresses[thread->CallStackDepth]);
    thread->ScriptBufferId = retBufferId;
  } else {
    ImpLog(LL_Error, LC_VM, "Return error, call stack empty.\n");
  }
}
VmInstruction(InstLoop) {
  StartInstruction;
  PopUint16(labelNum);
  PopExpression(loopCount);

  uint8_t* labelAdr = ScriptGetLabelAddress(
      thread->VmContext->ScriptBuffers[thread->ScriptBufferId], labelNum);

  if (thread->LoopLabelNum == labelNum) {
    loopCount = thread->LoopCounter;
  } else {
    thread->LoopLabelNum = labelNum;
    thread->LoopCounter = loopCount;
  }
  thread->LoopCounter--;
  if (thread->LoopCounter) {
    thread->Ip = labelAdr;
  } else {
    thread->LoopLabelNum = 0xFFFF;
  }
}
VmInstruction(InstFlagOnJump) {
  StartInstruction;
  PopUint8(value);
  PopExpression(flagId);
  PopUint16(labelNum);

  uint8_t* labelAdr = ScriptGetLabelAddress(
      thread->VmContext->ScriptBuffers[thread->ScriptBufferId], labelNum);

  if (thread->GameContext->GetFlag(flagId) == (bool)value) {
    thread->Ip = labelAdr;
  }
}
VmInstruction(InstKeyOnJump) {
  StartInstruction;
  PopUint8(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  PopUint16(labelNum);

  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction KeyOnJump(arg1: %i, arg2: %i, arg3: %i, "
             "labelNum: %i)\n",
             arg1, arg2, arg3, labelNum);
}
VmInstruction(InstLoadJump) {
  StartInstruction;
  PopExpression(scriptId);
  PopUint16(labelNum);
  thread->VmContext->LoadScript(thread->ScriptBufferId, scriptId);
  uint8_t* labelAdr = ScriptGetLabelAddress(
      thread->VmContext->ScriptBuffers[thread->ScriptBufferId], labelNum);
  thread->Ip = labelAdr;
}
VmInstruction(InstSwitch) {
  StartInstruction;
  PopExpression(switchVal);
  thread->VmContext->SwitchValue = switchVal;
}
VmInstruction(InstCase) {
  StartInstruction;
  PopExpression(caseVal);
  PopLocalLabel(labelAdr);

  if (thread->VmContext->SwitchValue == caseVal) {
    thread->Ip = labelAdr;
  }
}

}  // namespace Vm

}  // namespace Impacto