#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SystemMenu {
constexpr int MenuEntriesNumMax = 8;

inline uint32_t BackgroundColor;
inline Sprite BackgroundFilter;
inline glm::vec2 CircleStartPosition;
inline Sprite CircleSprite;
inline float CircleOffset;
inline glm::vec2 ErinPosition;
inline Sprite ErinSprite;
inline float TitleFadeInDuration;
inline float TitleFadeOutDuration;
inline uint32_t FocusTint;
inline glm::vec2 MenuEntriesPositions[MenuEntriesNumMax];

void Configure();

}  // namespace SystemMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto