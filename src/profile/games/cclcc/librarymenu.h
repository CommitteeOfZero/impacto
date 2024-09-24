#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace LibraryMenu {

inline Sprite BackgroundSprite;
inline Sprite LibraryBackgroundSprite;
inline glm::vec2 LibraryBackgroundPosition;
inline Sprite LibraryIndexSprite;
inline glm::vec2 LibraryIndexPosition;
inline Sprite LibraryButtonGuideSprite;
inline glm::vec2 LibraryButtonGuidePosition;
inline Sprite LibraryMaskSprite;

inline float FadeInDuration;
inline float FadeOutDuration;

void Configure();

}  // namespace LibraryMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto