#pragma once

#include "../util.h"
#include <ankerl/unordered_dense.h>

namespace Impacto {
namespace Profile {
namespace ScriptInput {

enum InputAxisDir {
  AxisPos = 1,
  AxisNeg = -1,
};

using KeyboardCodeType = std::underlying_type_t<SDL_Scancode>;
using ControllerButtonType = std::underlying_type_t<SDL_GameControllerButton>;
using ControllerAxisType = std::underlying_type_t<SDL_GameControllerAxis>;

inline ankerl::unordered_dense::map<int, std::vector<KeyboardCodeType>>
    PADToKeyboard;
inline ankerl::unordered_dense::map<int, uint32_t> PADToMouse;
inline ankerl::unordered_dense::map<int, ControllerButtonType> PADToController;
inline ankerl::unordered_dense::map<int,
                                    std::pair<ControllerAxisType, InputAxisDir>>
    PADToControllerAxis;

inline uint32_t PADcustomType;
inline uint32_t PADcustomSizeA;
inline uint32_t PADcustomSizeB;
inline uint32_t PADcustomA[65];
inline uint32_t PADcustomB[65];

void Configure();

}  // namespace ScriptInput
}  // namespace Profile
}  // namespace Impacto