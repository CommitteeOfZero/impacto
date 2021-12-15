#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace MovieMenu {

void Configure();

int const MaxMovieThumbnails = 10;

extern Sprite BackgroundSprite;

extern Sprite FirstOPTopPartSprite;
extern Sprite FirstOPBottomPartSprite;
extern Sprite SecondOPTopPartSprite;
extern Sprite SecondOPBottomPartSprite;
extern Sprite UnlockedMovieThumbnailSprites[MaxMovieThumbnails];
extern Sprite LockedMovieThumbnailSprites[MaxMovieThumbnails];
extern Sprite SelectionHighlightTopLeft;
extern Sprite SelectionHighlightTopRight;
extern Sprite SelectionHighlightBottomLeft;
extern Sprite SelectionHighlightBottomRight;
extern int ItemCount;
extern int ItemsPerRow;
extern glm::vec2 InitialItemPosition;
extern glm::vec2 ItemOffset;

extern float HighlightAnimationDuration;
extern glm::vec2 HighlightTopLeftOffset;
extern glm::vec2 HighlightTopRightOffset;
extern glm::vec2 HighlightBottomLeftOffset;
extern glm::vec2 HighlightBottomRightOffset;

extern float FadeInDuration;
extern float FadeOutDuration;

}  // namespace MovieMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto