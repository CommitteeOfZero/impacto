#pragma once

#include <ankerl/unordered_dense.h>

#include "../util.h"
#include "../inputsystem.h"
namespace Impacto {
namespace Profile {
namespace ScriptInput {

enum InputAxisDir {
  AxisPos = 1,
  AxisNeg = -1,
};

struct KeyboardPadMapping {
  enum InputMode : uint8_t {
    Tap = 0b01,
    Held = 0b10,
    Any = Tap | Held,
  };

  uint8_t Id{};
  InputMode Mode;
};

inline ankerl::unordered_dense::map<int, std::vector<KeyboardPadMapping>>
    PADToKBcustom;
inline ankerl::unordered_dense::map<int, uint32_t> PADToMouse;
inline ankerl::unordered_dense::map<int, Input::ControllerButton>
    PADToController;
inline ankerl::unordered_dense::map<
    int, std::pair<Input::ControllerAxis, InputAxisDir>>
    PADToControllerAxis;

inline std::vector<uint32_t> PADcustomA;
inline std::vector<uint32_t> PADcustomB;

void Configure();

}  // namespace ScriptInput
}  // namespace Profile
}  // namespace Impacto