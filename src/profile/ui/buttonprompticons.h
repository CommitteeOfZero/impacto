#pragma once

#include <ankerl/unordered_dense.h>

#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace ButtonPromptIcons {

inline ankerl::unordered_dense::map<int, Sprite> ControllerIcons;
inline ankerl::unordered_dense::map<int, Sprite> KeyboardIcons;

void Configure();

}  // namespace ButtonPromptIcons
}  // namespace Profile
}  // namespace Impacto
