#pragma once

#include <vector>

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace HelpMenu {

void Configure();

inline std::vector<Sprite> PageSprites;
// SW_SYSSUBMENUCT value at which the help is fully shown.
inline int CountMax;

}  // namespace HelpMenu
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
