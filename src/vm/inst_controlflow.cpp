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

  thread->IpOffset = labelAdr;
}
VmInstruction(InstJumpTable) {
  StartInstruction;
  PopExpression(labelNumIndex);
  PopUint16(dataLabelNum);
  uint32_t dataAdr =
      ScriptGetLabelAddress(thread->ScriptBufferId, dataLabelNum);
  uint8_t* dataPtr =
      &ScriptBuffers[thread->ScriptBufferId][dataAdr + 2 * labelNumIndex];
  uint32_t labelAdr = ScriptGetLabelAddress(
      thread->ScriptBufferId, SDL_SwapLE16(UnalignedRead<uint16_t>(dataPtr)));

  thread->IpOffset = labelAdr;
}
VmInstruction(InstIf) {
  StartInstruction;
  PopUint8(check);
  PopExpression(condition);

  PopUint16(labelNum);
  uint32_t labelAdr = ScriptGetLabelAddress(thread->ScriptBufferId, labelNum);

  if ((bool)check == (bool)condition) {
    thread->IpOffset = labelAdr;
  }

  // Hack: Pokecom infinite recursion in DaSH macrosys2
  if (thread->Variables[0] == 1 && labelNum == 107 &&
      thread->ScriptBufferId == 7) {
    thread->Variables[0] = 0;
    thread->IpOffset = labelAdr;
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
      thread->ReturnAddresses[thread->CallStackDepth] = thread->IpOffset;
    }
    thread->ReturnScriptBufferIds[thread->CallStackDepth++] =
        thread->ScriptBufferId;
    thread->IpOffset = labelAdr;
  } else {
    ImpLog(LogLevel::Error, LogChannel::VM,
           "Call error, call stack overflow.\n");
  }
}
VmInstruction(InstJumpFar) {
  StartInstruction;
  PopExpression(scriptBufferId);
  PopFarLabel(labelAdr, scriptBufferId);

  thread->ScriptBufferId = scriptBufferId;
  thread->IpOffset = labelAdr;
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
      thread->ReturnAddresses[thread->CallStackDepth] = thread->IpOffset;
    }
    thread->ReturnScriptBufferIds[thread->CallStackDepth++] =
        thread->ScriptBufferId;
    thread->IpOffset = labelAdr;
    thread->ScriptBufferId = scriptBufferId;
  } else {
    ImpLog(LogLevel::Error, LogChannel::VM,
           "CallFar error, call stack overflow.\n");
  }
}
VmInstruction(InstReturn) {
  StartInstruction;
  if (thread->CallStackDepth) {
    thread->CallStackDepth--;
    uint32_t retBufferId =
        thread->ReturnScriptBufferIds[thread->CallStackDepth];
    if (Profile::Vm::UseReturnIds) {
      thread->IpOffset = ScriptGetRetAddress(
          retBufferId, thread->ReturnIds[thread->CallStackDepth]);
    } else {
      thread->IpOffset = thread->ReturnAddresses[thread->CallStackDepth];
    }
    thread->ScriptBufferId = retBufferId;
  } else {
    ImpLog(LogLevel::Error, LogChannel::VM,
           "Return error, call stack empty.\n");
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
        thread->IpOffset = ScriptGetRetAddress(
            retBufferId, thread->ReturnIds[thread->CallStackDepth]);
      } else {
        thread->IpOffset = thread->ReturnAddresses[thread->CallStackDepth];
      }
      thread->ScriptBufferId = retBufferId;
    }
  } else {
    ImpLog(LogLevel::Error, LogChannel::VM,
           "Return error, call stack empty.\n");
  }
}
VmInstruction(InstLoop) {
  StartInstruction;
  PopUint16(labelNum);
  PopExpression(loopCount);

  uint32_t labelAdr = ScriptGetLabelAddress(thread->ScriptBufferId, labelNum);

  if (thread->LoopLabelNum == labelNum) {
    loopCount = thread->LoopCounter;
  } else {
    thread->LoopLabelNum = labelNum;
    thread->LoopCounter = loopCount;
  }
  thread->LoopCounter--;
  if (thread->LoopCounter) {
    thread->IpOffset = labelAdr;
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
    thread->IpOffset = labelAdr;
  }
}
VmInstruction(InstKeyOnJump) {
  using namespace Interface;

  StartInstruction;
  PopUint8(type);
  PopExpression(buttonsArg);
  PopExpression(downTypeId);
  PopUint16(labelNum);
  uint32_t labelAdr = ScriptGetLabelAddress(thread->ScriptBufferId, labelNum);

  const InputDownType downType = static_cast<InputDownType>(downTypeId);

  const bool inputResult = [&]() -> bool {
    const uint32_t bitfield = (type & 2) ? PADcustom[buttonsArg] : buttonsArg;

    uint32_t padInputDown = GetPadInputButtonDown(downType);
    if (downType == InputDownType::IsDown) {
      padInputDown |= PADinputMouseIsDown;
    } else if (downType == InputDownType::WentDown) {
      padInputDown |= PADinputMouseWentDown;
    }

    return bitfield & padInputDown;
  }();

  if (inputResult) {
    thread->IpOffset = labelAdr;
  }

  ImpLogSlow(LogLevel::Trace, LogChannel::VM,
             "KeyOnJump(type: {:d}, buttons: {:d}, downType: {:d}, "
             "labelNum: {:d})\n",
             type, buttonsArg, downTypeId, labelNum);
}
VmInstruction(InstKeyOnJump_Dash) {
  StartInstruction;
  PopUint8(arg1);
  PopExpression(arg2);
  if (arg1 < 10) {
    PopExpression(arg3);
  }
  PopUint16(labelNum);
  uint32_t labelAdr = ScriptGetLabelAddress(thread->ScriptBufferId, labelNum);
  if (arg1 & 2) {
    arg2 = Interface::PADcustom[arg2];
  }
  if (arg2 & Interface::PADinputButtonWentDown ||
      arg2 & Interface::PADinputMouseWentDown) {
    thread->IpOffset = labelAdr;
    Interface::PADinputButtonWentDown = 0;
    Interface::PADinputMouseWentDown = 0;
  }

  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction KeyOnJump(arg1: {:d}, arg2: {:d}, "
             "labelNum: {:d})\n",
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
  uint32_t labelAdr = ScriptGetLabelAddress(thread->ScriptBufferId, labelNum);
  if (Input::KeyboardButtonWentDown[SDL_SCANCODE_D]) {
    thread->IpOffset = labelAdr;
  }
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction ClickOnJump(arg1: {:d}, arg2: {:d}, "
             "labelNum: {:d})\n",
             arg1, arg2, labelNum);
}
VmInstruction(InstKeyboardOnJump) {
  StartInstruction;
  PopUint8(arg1);
  PopExpression(arg2);
  PopExpression(arg3);
  PopLocalLabel(arg4);
  (void)arg4;  // Unused
  ImpLogSlow(
      LogLevel::Warning, LogChannel::VMStub,
      "STUB instruction KeyboardOnJump(arg1: {:d}, arg2: {:d}, arg3: {:d}, "
      "arg4: {:d})\n",
      arg1, arg2, arg3, arg4);
}
VmInstruction(InstControlOnJump) {
  StartInstruction;
  PopUint8(controlStateTarget);
  PopExpression(controlId);
  PopLocalLabel(labelAdr);
  if ((bool)controlStateTarget == Interface::GetControlState(controlId)) {
    thread->IpOffset = labelAdr;
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
  uint32_t labelAdr = ScriptGetLabelAddress(thread->ScriptBufferId, labelNum);
  thread->IpOffset = labelAdr;
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
    thread->IpOffset = labelAdr;
  }
}

VmInstruction(InstFlagOffReturn) {
  StartInstruction;
  PopUint8(arg1);
  PopExpression(arg2);
  if ((bool)arg1 == GetFlag(arg2)) {
    if (thread->CallStackDepth) {
      thread->CallStackDepth--;
      uint32_t retBufferId =
          thread->ReturnScriptBufferIds[thread->CallStackDepth];
      if (Profile::Vm::UseReturnIds) {
        thread->IpOffset = ScriptGetRetAddress(
            retBufferId, thread->ReturnIds[thread->CallStackDepth]);
      } else {
        thread->IpOffset = thread->ReturnAddresses[thread->CallStackDepth];
      }
      thread->ScriptBufferId = retBufferId;
    } else {
      ImpLog(LogLevel::Error, LogChannel::VM,
             "Return error, call stack empty.\n");
    }
  }
}

}  // namespace Vm

}  // namespace Impacto