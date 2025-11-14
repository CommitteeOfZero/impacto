#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace DelusionTrigger {

inline Sprite BackgroundSprite;
inline Sprite BackgroundSpriteMask;
inline Sprite ScreenMask;
inline std::array<std::vector<Sprite>, 21> DelusionTextGlyphs;

void Configure();

}  // namespace DelusionTrigger
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto