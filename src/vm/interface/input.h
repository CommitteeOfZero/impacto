#pragma once

#include "../../impacto.h"

namespace Impacto {
namespace Vm {
namespace Interface {

enum PADinput {
  PAD1UP = 1,
  PAD1DOWN = 2,
  PAD1LEFT = 4,
  PAD1RIGHT = 8,
  PAD1START = 0x10,
  PAD1SELECT = 0x20,
  PAD1L3 = 0x40,
  PAD1R3 = 0x80,
  PAD1L1 = 0x100,
  PAD1R1 = 0x200,
  PAD1L2 = 0x400,
  PAD1R2 = 0x800,
  PAD1A = 0x1000,
  PAD1B = 0x2000,
  PAD1X = 0x4000,
  PAD1Y = 0x8000
};

// Type A control bindings, used by scripts
static uint32_t PADcustom[] = {0x10000,
                               0x20000,
                               0x40000,
                               0x80000,
                               PAD1A | PAD1START,
                               PAD1A,
                               PAD1B,
                               PAD1R1,
                               PAD1START,
                               PAD1SELECT,
                               PAD1X,
                               0,
                               0, //temp disable PAD1L2
                               PAD1R3,
                               PAD1A | PAD1B | PAD1START,
                               PAD1A,
                               PAD1B,
                               PAD1X,
                               PAD1Y,
                               PAD1R1,
                               PAD1R2,
                               PAD1L1,
                               PAD1Y,
                               0xF1000,
                               PAD1R1,
                               PAD1L1,
                               PAD1A,
                               PAD1B,
                               0x100000 | PAD1UP,
                               0x200000 | PAD1DOWN,
                               0x400000 | PAD1LEFT,
                               0x800000 | PAD1RIGHT,
                               0x1000000,
                               0x2000000,
                               0x4000000,
                               0x8000000,
                               0x20000000,
                               0x40000000,
                               0x80000000,
                               0,
                               0};

extern int PADinputButtonWentDown;
extern int PADinputMouseWentDown;
extern int PADinputButtonIsDown;
extern int PADinputMouseIsDown;

void UpdatePADInput();

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto