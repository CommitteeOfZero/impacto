#pragma once

#include <span>

#include "../../impacto.h"
#include "../../inputsystem.h"

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

inline ankerl::unordered_dense::map<uint8_t,
                                    std::vector<Input::KeyboardScanCode>>
    KBcustom{};
inline std::array<bool, SDL_SCANCODE_COUNT> KBinputHeldDown = {false};

namespace ControlType {
enum ControlTypeEnum {
  OK = 0,
  Back = 2,
  Hide = 8,
  NextMessage = 21,
  ForceSkip = 22,
  SkipMode = 23,
  AutoMode = 24,
  QuickSave = 26,
  MainMenu = 28,
  Backlog = 29,
  Tips = 30,
  MovieCancel = 38,
  LogoSkip = 39,
  ResetOptions = 40,
  DelusionTriggerL = 41,
  DelusionTriggerR = 42,
};
}

uint32_t GetPadInputButtonDown(InputDownType downType);
void UpdatePADInput();
void UpdatePADHoldInput(float dt);
void ResetPADHoldTimer(uint32_t PADcode);
void UpdatePADcustomType(int type);
bool GetControlState(ControlType::ControlTypeEnum controlId,
                     InputDownType downType = InputDownType::WentDown);
void UpdateKBInput();
void UpdateKBHoldInput(float dt);

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto