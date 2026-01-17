#pragma once

#include "../../impacto.h"
#include <span>

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
  PAD1Y = 0x8000,
  PAD1UP_LS = 0x100000,
  PAD1DOWN_LS = 0x200000,
  PAD1LEFT_LS = 0x400000,
  PAD1RIGHT_LS = 0x800000,
  PAD1UP_RS = 0x1000000,
  PAD1DOWN_RS = 0x2000000,
  PAD1LEFT_RS = 0x4000000,
  PAD1RIGHT_RS = 0x8000000,
  PAD1UP_DIRECT = 0x1,
  PAD1DOWN_DIRECT = 0x2,
  PAD1LEFT_DIRECT = 0x4,
  PAD1RIGHT_DIRECT = 0x8,
};

inline std::span<uint32_t> PADcustom;

enum class InputDownType {
  IsDown = 0,
  WentDown = 1,
  RepeatDown = 2,
  RepeatAccelDown = 3,
};

inline uint32_t PADinputButtonWentDown = 0;         // padone
inline uint32_t PADinputButtonIsDown = 0;           // padrep
inline uint32_t PADinputButtonRepeatDown = 0;       // padref
inline uint32_t PADinputButtonRepeatAccelDown = 0;  // padacc
inline uint32_t PADinputMouseWentDown = 0;
inline uint32_t PADinputMouseIsDown = 0;

enum ControlType {
  CT_OK = 0,
  CT_Back = 2,
  CT_HIDE = 8,
  CT_NextMessage = 21,
  CT_ForceSkip = 22,
  CT_SkipMode = 23,
  CT_AutoMode = 24,
  CT_QuickSave = 26,
  CT_MainMenu = 28,
  CT_Backlog = 29,
  CT_Tips = 30,
  CT_MovieCancel = 38,
  CT_LogoSkip = 39,
  CT_ResetOptions = 40,
  CT_DelusionTriggerL = 41,
  CT_DelusionTriggerR = 42,
};

uint32_t GetPadInputButtonDown(InputDownType downType);
void UpdatePADInput();
void UpdatePADHoldInput(float dt);
void ResetPADHoldTimer(uint32_t PADcode);
void UpdatePADcustomType(int type);
bool GetControlState(int controlId,
                     InputDownType downType = InputDownType::WentDown);

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto