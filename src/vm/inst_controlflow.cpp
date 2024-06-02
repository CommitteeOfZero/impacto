#include "inst_controlflow.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../mem.h"
#include "../log.h"
#include "../profile/vm.h"
#include "../inputsystem.h"
#include "interface/input.h"

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
      ScriptBuffers[thread->ScriptBufferId], dataLabelNum);
  uint8_t* labelAdr = ScriptGetLabelAddress(
      ScriptBuffers[thread->ScriptBufferId],
      SDL_SwapLE16(UnalignedRead<uint16_t>(dataAdr + 2 * labelNumIndex)));

  thread->Ip = labelAdr;
}
VmInstruction(InstIf) {
  StartInstruction;
  PopUint8(check);
  PopExpression(condition);

  PopUint16(labelNum);
  uint8_t* labelAdr =
      ScriptGetLabelAddress(ScriptBuffers[thread->ScriptBufferId], labelNum);

  if ((bool)check == (bool)condition) {
    thread->Ip = labelAdr;
  }

  // Hack: Pokecom infinite recursion in DaSH macrosys2
  if (thread->Variables[0] == 1 && labelNum == 107 &&
      thread->ScriptBufferId == 7) {
    thread->Variables[0] = 0;
    thread->Ip = labelAdr;
  }
}
VmInstruction(InstCall) {
  StartInstruction;
  PopLocalLabel(labelAdr);

  if (thread->CallStackDepth != MaxCallStackDepth) {
    if (Profile::Vm::UseReturnIds) {
      PopUint16(retNum);
      thread->ReturnIds[thread->CallStackDepth] = retNum;
    } else {
      thread->ReturnAddresses[thread->CallStackDepth] = thread->Ip;
    }
    thread->ReturnScriptBufferIds[thread->CallStackDepth++] =
        thread->ScriptBufferId;
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

  if (thread->CallStackDepth != MaxCallStackDepth) {
    if (Profile::Vm::UseReturnIds) {
      PopUint16(retNum);
      thread->ReturnIds[thread->CallStackDepth] = retNum;
    } else {
      thread->ReturnAddresses[thread->CallStackDepth] = thread->Ip;
    }
    thread->ReturnScriptBufferIds[thread->CallStackDepth++] =
        thread->ScriptBufferId;
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
    uint32_t retBufferId =
        thread->ReturnScriptBufferIds[thread->CallStackDepth];
    if (Profile::Vm::UseReturnIds) {
      thread->Ip =
          ScriptGetRetAddress(ScriptBuffers[retBufferId],
                              thread->ReturnIds[thread->CallStackDepth]);
    } else {
      thread->Ip = thread->ReturnAddresses[thread->CallStackDepth];
    }
    thread->ScriptBufferId = retBufferId;
  } else {
    ImpLog(LL_Error, LC_VM, "Return error, call stack empty.\n");
  }
}
VmInstruction(InstReturnIfFlag) {
  StartInstruction;
  PopUint8(value);
  PopExpression(flagId);
  if (thread->CallStackDepth) {
    if (GetFlag(flagId) == (bool)value) {
      thread->CallStackDepth--;
      uint32_t retBufferId =
          thread->ReturnScriptBufferIds[thread->CallStackDepth];
      if (Profile::Vm::UseReturnIds) {
        thread->Ip =
            ScriptGetRetAddress(ScriptBuffers[retBufferId],
                                thread->ReturnIds[thread->CallStackDepth]);
      } else {
        thread->Ip = thread->ReturnAddresses[thread->CallStackDepth];
      }
      thread->ScriptBufferId = retBufferId;
    }
  } else {
    ImpLog(LL_Error, LC_VM, "Return error, call stack empty.\n");
  }
}
VmInstruction(InstLoop) {
  StartInstruction;
  PopUint16(labelNum);
  PopExpression(loopCount);

  uint8_t* labelAdr =
      ScriptGetLabelAddress(ScriptBuffers[thread->ScriptBufferId], labelNum);

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
  PopLocalLabel(labelAdr);

  if (GetFlag(flagId) == (bool)value) {
    thread->Ip = labelAdr;
  }
}
VmInstruction(InstKeyOnJump) {
  StartInstruction;
  PopUint8(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  PopUint16(labelNum);
  uint8_t* labelAdr =
      ScriptGetLabelAddress(ScriptBuffers[thread->ScriptBufferId], labelNum);

  if (arg1 & 2) {
    arg2 = Interface::PADcustom[arg2];
  }
  if (arg2 & Interface::PADinputButtonWentDown ||
      arg2 & Interface::PADinputMouseWentDown) {
    thread->Ip = labelAdr;
    Interface::PADinputButtonWentDown = 0;
    Interface::PADinputMouseWentDown = 0;
  }

  ImpLogSlow(LL_Trace, LC_VM,
             "KeyOnJump(arg1: %i, arg2: %i, arg3: %i, "
             "labelNum: %i)\n",
             arg1, arg2, arg3, labelNum);
}
VmInstruction(InstKeyOnJump_Dash) {
  StartInstruction;
  PopUint8(arg1);
  PopExpression(arg2);
  if (arg1 < 10) {
    PopExpression(arg3);
  }
  PopUint16(labelNum);
  uint8_t* labelAdr =
      ScriptGetLabelAddress(ScriptBuffers[thread->ScriptBufferId], labelNum);
  if (arg1 & 2) {
    arg2 = Interface::PADcustom[arg2];
  }
  if (arg2 & Interface::PADinputButtonWentDown ||
      arg2 & Interface::PADinputMouseWentDown) {
    thread->Ip = labelAdr;
    Interface::PADinputButtonWentDown = 0;
    Interface::PADinputMouseWentDown = 0;
  }

  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction KeyOnJump(arg1: %i, arg2: %i, "
             "labelNum: %i)\n",
             arg1, arg2, labelNum);
}
VmInstruction(InstClickOnJump) {
  StartInstruction;
  PopUint8(arg1);
  if ((arg1 & 0xFE) == 2) {
    PopExpression(_arg1);
    PopExpression(_arg2);
    PopExpression(_arg3);
  }
  PopExpression(arg2);
  PopUint16(labelNum);
  uint8_t* labelAdr =
      ScriptGetLabelAddress(ScriptBuffers[thread->ScriptBufferId], labelNum);
  if (Input::KeyboardButtonWentDown[SDL_SCANCODE_D]) {
    thread->Ip = labelAdr;
  }
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction ClickOnJump(arg1: %i, arg2: %i, "
             "labelNum: %i)\n",
             arg1, arg2, labelNum);
}
VmInstruction(InstKeyboardOnJump) {
  StartInstruction;
  PopUint8(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  PopLocalLabel(arg4);
  (void)arg4;  // Unused
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction KeyboardOnJump(arg1: %i, arg2: %i, arg3: %i, "
             "arg4: %i)\n",
             arg1, arg2, arg3, arg4);
}
VmInstruction(InstControlOnJump) {
  StartInstruction;
  PopUint8(controlStateTarget);
  PopExpression(controlId);
  PopLocalLabel(labelAdr);
  if ((bool)controlStateTarget == Interface::GetControlState(controlId)) {
    thread->Ip = labelAdr;
  }
}
VmInstruction(InstGetControl) { StartInstruction; }
VmInstruction(InstPackFileAddBind) { StartInstruction; }
VmInstruction(InstLoadJump) {
  StartInstruction;
  PopExpression(scriptId);
  PopUint16(labelNum);
  if (Profile::Vm::UseMsbStrings) {
    LoadMsb(thread->ScriptBufferId, scriptId);
    if (!Profile::Vm::UseSeparateMsbArchive) scriptId += 1;
  }
  LoadScript(thread->ScriptBufferId, scriptId);
  uint8_t* labelAdr =
      ScriptGetLabelAddress(ScriptBuffers[thread->ScriptBufferId], labelNum);
  thread->Ip = labelAdr;
}
VmInstruction(InstSwitch) {
  StartInstruction;
  PopExpression(switchVal);
  SwitchValue = switchVal;
}
VmInstruction(InstCase) {
  StartInstruction;
  PopExpression(caseVal);
  PopLocalLabel(labelAdr);

  if (static_cast<int>(SwitchValue) == caseVal) {
    thread->Ip = labelAdr;
  }
}

}  // namespace Vm

}  // namespace Impacto