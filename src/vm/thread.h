#pragma once

#include "../impacto.h"

//#include "vm.h"
//#include "../game.h"

namespace Impacto {

class Game;

namespace Vm {

enum ThreadFlagState {
  None = 0x0,
  Destroy = 0x8000000,
  Animate = 0x10000000,
  Display = 0x20000000,
  Pause = 0x40000000,
  Message = 0x80000000,
};

enum ThreadDrawType {
  Text = 0x0,
  Main = 0x1,
  Review = 0x2,
  Mask = 0x3,
  SystemText = 0x4,
  SaveMenu = 0x5,
  MainChip = 0x6,
  TitleMenu = 0x7,
  CvMenu = 0x8,
  Option = 0x9,
  SystemMenu = 0xA,
  SystemMessage = 0xB,
  PlayData = 0xC,
  Album = 0xD,
  MusicMode = 0xE,
  MovieMode = 0x10,
  SaveIcon = 0x12,
  GlobalSystemMessage = 0x15,
  DebugEditor = 0x1E,
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
  uint8_t* ReturnAdresses[8];
  uint32_t ReturnGroupIds[8];
  uint32_t DrawPriority;
  uint32_t DrawType;
  uint32_t Alpha;
  uint32_t Temp1;
  uint32_t Temp2;
  uint32_t Variables[32];
  uint32_t MesId;
  Vm* VmContext;
  Game* GameContext;
};

}  // namespace Vm

}  // namespace Impacto