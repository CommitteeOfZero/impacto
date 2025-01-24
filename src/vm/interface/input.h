#pragma once

#include "../../impacto.h"
#include "../../vendor/span/span.hpp"

namespace Impacto {
namespace Vm {
namespace Interface {
// Might have to take this from lua file if older games use different consts
enum PADinput {
  PAD1UP = 0x10000,
  PAD1DOWN = 0x20000,
  PAD1LEFT = 0x40000,
  PAD1RIGHT = 0x80000,
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

inline tcb::span<uint32_t> PADcustom;

inline int PADinputButtonWentDown = 0;
inline int PADinputMouseWentDown = 0;
inline int PADinputButtonIsDown = 0;
inline int PADinputMouseIsDown = 0;

enum ControlType {
  CT_OK = 0,
  CT_Back = 2,
  CT_HIDE = 8,
  CT_NextMessage = 21,
  CT_QuickSave = 26,
  CT_MainMenu = 28,
  CT_Backlog = 29,
  CT_Tips = 30,
  CT_LogoSkip = 39,
  CT_ResetOptions = 40,
  CT_DelusionTriggerL = 41,
  CT_DelusionTriggerR = 42,
};

void UpdatePADInput();
void UpdatePADcustomType(int type);
bool GetControlState(int controlId);

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto