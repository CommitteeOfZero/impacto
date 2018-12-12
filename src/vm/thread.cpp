#include "thread.h"

namespace Impacto {
namespace Vm {

void* Sc3VmThread::GetMemberPointer(uint32_t offset) {
  switch (offset) {
    case TO_Flags:
      return &Flags;
    case TO_ExecPri:
      return &ExecPriority;
    case TO_ScrBuf:
      return &ScriptBufferId;
    case TO_WaitCount:
      return &WaitCounter;
    case TO_ScrParam:
      return &ScriptParam;
    case TO_ScrAddr:
      return &Ip;
    case TO_LoopCount:
      return &LoopCounter;
    case TO_LoopAddr:
      return &LoopLabelNum;
    case TO_RetCount:
      return &CallStackDepth;
    case TO_RetAddr:
      return &ReturnAdresses;
    case TO_RetScrBuf:
      return &ReturnScriptBufferIds;
    case TO_DrawPri:
      return &DrawPriority;
    case TO_DrawType:
      return &DrawType;
    case TO_Alpha:
      return &Alpha;
    case TO_Temp1:
      return &Temp1;
    case TO_Temp2:
      return &Temp2;
    default:
      return &Variables[offset - TO_ThdVarBegin];
      break;
  }
}
}  // namespace Vm
}  // namespace Impacto