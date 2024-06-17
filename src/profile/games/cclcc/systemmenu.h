#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace SystemMenu {
constexpr int MenuEntriesNumMax = 16;

inline Sprite MenuButtonGuide;
inline float TitleFadeInDuration;
inline float TitleFadeOutDuration;
inline glm::vec2 MenuEntriesPositions[MenuEntriesNumMax];

void Configure();

}  // namespace SystemMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto