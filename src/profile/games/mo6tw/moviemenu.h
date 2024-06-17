#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace MovieMenu {

void Configure();

int constexpr MaxMovieThumbnails = 10;

inline Sprite BackgroundSprite;

inline Sprite FirstOPTopPartSprite;
inline Sprite FirstOPBottomPartSprite;
inline Sprite SecondOPTopPartSprite;
inline Sprite SecondOPBottomPartSprite;
inline Sprite UnlockedMovieThumbnailSprites[MaxMovieThumbnails];
inline Sprite LockedMovieThumbnailSprites[MaxMovieThumbnails];
inline Sprite SelectionHighlightTopLeft;
inline Sprite SelectionHighlightTopRight;
inline Sprite SelectionHighlightBottomLeft;
inline Sprite SelectionHighlightBottomRight;
inline int ItemCount;
inline int ItemsPerRow;
inline glm::vec2 InitialItemPosition;
inline glm::vec2 ItemOffset;

inline float HighlightAnimationDuration;
inline glm::vec2 HighlightTopLeftOffset;
inline glm::vec2 HighlightTopRightOffset;
inline glm::vec2 HighlightBottomLeftOffset;
inline glm::vec2 HighlightBottomRightOffset;

inline float FadeInDuration;
inline float FadeOutDuration;

}  // namespace MovieMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto