#pragma once

#include <ankerl/unordered_dense.h>
#include <variant>

#include "../../inputsystem.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace ButtonPromptIcons {

inline ankerl::unordered_dense::map<
    std::variant<Input::ControllerButton, std::string>, Sprite>
    ControllerIcons;
inline ankerl::unordered_dense::map<
    std::variant<Input::KeyboardScanCode, std::string>, Sprite>
    KeyboardIcons;

void Configure();

}  // namespace ButtonPromptIcons
}  // namespace Profile
}  // namespace Impacto
