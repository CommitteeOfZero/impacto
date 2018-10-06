#pragma once

#include "../impacto.h"

//#include "vm.h"
//#include "../game.h"

namespace Impacto {

class Game;

namespace Vm {

int const VmCallStackDepth = 8;
int const VmMaxThreadVars = 32;

enum ThreadFlagState {
  TF_None = 0x0,
  TF_Destroy = 0x8000000,
  TF_Animate = 0x10000000,
  TF_Display = 0x20000000,
  TF_Pause = 0x40000000,
  TF_Message = 0x80000000,
};

enum ThreadDrawType {
  TD_Text = 0x0,
  TD_Main = 0x1,
  TD_Review = 0x2,
  TD_Mask = 0x3,
  TD_SystemText = 0x4,
  TD_SaveMenu = 0x5,
  TD_MainChip = 0x6,
  TD_TitleMenu = 0x7,
  TD_CvMenu = 0x8,
  TD_Option = 0x9,
  TD_SystemMenu = 0xA,
  TD_SystemMessage = 0xB,
  TD_PlayData = 0xC,
  TD_Album = 0xD,
  TD_MusicMode = 0xE,
  TD_MovieMode = 0x10,
  TD_SaveIcon = 0x12,
  TD_GlobalSystemMessage = 0x15,
  TD_DebugEditor = 0x1E,
};

enum ThreadOffset {
  TO_Flag = 0,
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

typedef struct Sc3VmThread Sc3VmThread;
class Vm;

struct Sc3VmThread {
  uint32_t Id;
  uint32_t Flag;
  Sc3VmThread* PreviousContext;
  Sc3VmThread* NextContext;
  Sc3VmThread* NextFreeContext;
  uint32_t ExecPriority;
  uint32_t ScriptBufferId;
  uint32_t WaitCounter;
  uint32_t ScriptParam;
  uint8_t* Ip;
  uint32_t LoopCounter;
  uint8_t* LoopAddress;
  uint32_t ReturnCount;
  uint16_t ReturnAdresses[VmCallStackDepth];
  uint32_t ReturnGroupIds[VmCallStackDepth];
  uint32_t DrawPriority;
  uint32_t DrawType;
  uint32_t Alpha;
  uint32_t Temp1;
  uint32_t Temp2;
  uint32_t Variables[VmMaxThreadVars];
  uint32_t MesId;
  Vm* VmContext;
  Game* GameContext;
};

}  // namespace Vm

}  // namespace Impacto