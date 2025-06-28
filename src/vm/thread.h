#pragma once

#include "../impacto.h"

namespace Impacto {

namespace Vm {

int constexpr MaxCallStackDepth = 8;
int constexpr MaxThreadVars = 32;

enum ThreadStateFlag {  // Applies to both individual threads and thread groups
  TF_None = 0x0,
  TF_Destroy = 0x8000000,
  TF_Animate = 0x10000000,
  TF_Display = 0x20000000,
  TF_Pause = 0x40000000,
  TF_Message = 0x80000000,
};

enum ThreadGroupControlType {
  TC_Destroy = 0,
  TC_Pause = 1,
  TC_Start = 2,
  TC_Hide = 3,
  TC_Display = 4
};

enum ThreadMemberOffset {
  TO_Flags = 0,
  TO_ExecPri = 4,
  TO_ScrBuf = 5,
  TO_WaitCount = 6,
  TO_ScrParam = 7,
  TO_ScrAddr = 8,
  TO_LoopCount = 9,
  TO_LoopAddr = 10,
  TO_RetCount = 11,
  TO_RetAddr = 12,
  TO_RetScrBuf = 20,
  TO_DrawPri = 32,
  TO_DrawType = 33,
  TO_Alpha = 34,
  TO_Temp1 = 45,
  TO_Temp2 = 46,
  TO_ThdVarBegin = 47
};

struct BufferOffsetContext {
  uint32_t ScriptBufferId;
  uint32_t IpOffset;
};

class Vm;

struct Sc3VmThread {
  uint32_t Id;
  uint32_t Flags;
  Sc3VmThread* PreviousContext;
  Sc3VmThread* NextContext;
  Sc3VmThread* NextFreeContext;
  uint32_t ExecPriority;
  uint32_t ScriptBufferId;
  uint32_t GroupId;
  uint32_t WaitCounter;
  uint32_t ScriptParam;
  uint32_t IpOffset;
  uint32_t LoopCounter;
  uint16_t LoopLabelNum;
  uint32_t CallStackDepth;
  union {
    uint32_t ReturnAddresses[MaxCallStackDepth];
    uint16_t ReturnIds[MaxCallStackDepth];
  };
  uint32_t ReturnScriptBufferIds[MaxCallStackDepth];
  uint32_t DrawPriority;
  uint8_t DrawType;
  uint32_t Alpha;
  uint32_t Temp1;
  uint32_t Temp2;
  uint32_t Variables[MaxThreadVars];
  uint32_t DialoguePageId;

  void* GetMemberPointer(uint32_t offset);
  uint8_t* GetIp() const;
  void SetIp(uint8_t* ptr);
  void SetIp(BufferOffsetContext ctx);
};

}  // namespace Vm

}  // namespace Impacto