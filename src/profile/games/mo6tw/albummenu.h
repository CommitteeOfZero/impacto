#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace AlbumMenu {

void Configure();

int const CharacterButtonCount = 8;
int const CharacterPortraitCount = 5;
int const CharacterCount = 6;
int const EventCgCount = 104;

extern Sprite BackgroundSprite;

extern Sprite CharacterButtonSprites[CharacterButtonCount];
extern Sprite HighlightedCharacterButtonSprites[CharacterButtonCount];
extern glm::vec2 InitialButtonPosition;
extern float ButtonOddX;
extern float ButtonEvenX;
extern glm::vec2 ButtonMargin;
extern float HighlightAnimationDuration;
extern int YunoButtonIdx;
extern int SuzuButtonIdx;
extern Sprite CharacterPortraits[CharacterPortraitCount];
extern Sprite OthersPortraitTopPart;
extern Sprite OthersPortraitBottomPart;
extern glm::vec2 PortraitPosition;
extern glm::vec2 OthersPortraitPosition;
extern Sprite Thumbnails[EventCgCount];
extern int ThumbnailOffsets[CharacterCount];
extern Sprite LockedThumbnail;
extern Sprite ThumbnailBorder;
extern Sprite ThumbnailHighlightTopLeft;
extern Sprite ThumbnailHighlightTopRight;
extern Sprite ThumbnailHighlightBottomLeft;
extern Sprite ThumbnailHighlightBottomRight;
extern glm::vec2 ThumbnailGridFirstPosition;
extern glm::vec2 ThumbnailGridMargin;
extern RectF ThumbnailGridBounds;

extern float FadeInDuration;
extern float FadeOutDuration;

}  // namespace AlbumMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto