#include "inst_controlflow.h"

#include "expression.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstJump) {}
VmInstruction(InstJumpTable) {}
VmInstruction(InstIf) {
  thread->Ip += 2;
  PopUint8(check);
  PopExpression(condition);
  PopUint16(labelNum);
  uint8_t* labelAdr = ScriptGetLabelAddress(
      thread->VmContext->ScriptBuffers[thread->ScriptBufferId], labelNum);

  if (!check) {
    if (!condition) thread->Ip = labelAdr;
  } else if (condition)
    thread->Ip = labelAdr;
}
VmInstruction(InstCall) {}
VmInstruction(InstJumpFar) {}
VmInstruction(InstCallFar) {
  thread->Ip += 2;
  PopExpression(scriptBufferId);
  PopUint16(labelNum);
  PopUint16(retNum);
  uint8_t* labelAdr = ScriptGetLabelAddress(
      thread->VmContext->ScriptBuffers[scriptBufferId], labelNum);

  if (thread->ReturnCount != VmCallStackDepth) {
    thread->ReturnAdresses[thread->ReturnCount] = retNum;
    thread->ReturnGroupIds[thread->ReturnCount++] = thread->ScriptBufferId;
    thread->Ip = labelAdr;
    thread->ScriptBufferId = scriptBufferId;
  }
}
VmInstruction(InstReturn) {
  if (thread->ReturnCount) {
    thread->ReturnCount--;
    uint32_t retBufferId = thread->ReturnGroupIds[thread->ReturnCount];
    thread->Ip =
        ScriptGetRetAddress(thread->VmContext->ScriptBuffers[retBufferId],
                            thread->ReturnAdresses[thread->ReturnCount]);
    thread->ScriptBufferId = retBufferId;
  }
}
VmInstruction(InstLoop) {}
VmInstruction(InstFlagOnJump) {}
VmInstruction(InstKeyOnJump) {}
VmInstruction(InstLoadJump) {}
VmInstruction(InstSwitch) {}
VmInstruction(InstCase) {}

}  // namespace Vm

}  // namespace Impacto