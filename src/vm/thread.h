#pragma once

#include "../impacto.h"

//#include "vm.h"
//#include "../game.h"

namespace Impacto {

class Game;
enum DrawComponentType : uint8_t;

namespace Vm {

int const VmMaxCallStackDepth = 8;
int const VmMaxThreadVars = 32;

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
  uint8_t* Ip;
  uint32_t LoopCounter;
  uint16_t LoopLabelNum;
  uint32_t CallStackDepth;
  uint16_t ReturnAdresses[VmMaxCallStackDepth];
  uint32_t ReturnScriptBufferIds[VmMaxCallStackDepth];
  uint32_t DrawPriority;
  DrawComponentType DrawType;
  uint32_t Alpha;
  uint32_t Temp1;
  uint32_t Temp2;
  uint32_t Variables[VmMaxThreadVars];
  uint32_t DialoguePageId;
  Vm* VmContext;
  Game* GameContext;

  void* GetMemberPointer(uint32_t offset);
};

}  // namespace Vm

}  // namespace Impacto