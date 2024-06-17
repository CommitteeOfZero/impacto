#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace AlbumMenu {

void Configure();

int constexpr CharacterButtonCount = 8;
int constexpr CharacterPortraitCount = 5;
int constexpr CharacterCount = 6;
int constexpr EventCgCount = 104;

inline Sprite BackgroundSprite;

inline Sprite CharacterButtonSprites[CharacterButtonCount];
inline Sprite HighlightedCharacterButtonSprites[CharacterButtonCount];
inline glm::vec2 InitialButtonPosition;
inline float ButtonOddX;
inline float ButtonEvenX;
inline glm::vec2 ButtonMargin;
inline float HighlightAnimationDuration;
inline int YunoButtonIdx;
inline int SuzuButtonIdx;
inline Sprite CharacterPortraits[CharacterPortraitCount];
inline Sprite OthersPortraitTopPart;
inline Sprite OthersPortraitBottomPart;
inline glm::vec2 PortraitPosition;
inline glm::vec2 OthersPortraitPosition;
inline int ThumbnailsPerRow;
inline int ThumbnailsPerColumn;
inline Sprite Thumbnails[EventCgCount];
inline int ThumbnailOffsets[CharacterCount];
inline Sprite LockedThumbnail;
inline Sprite ThumbnailBorder;
inline Sprite ThumbnailHighlightTopLeft;
inline Sprite ThumbnailHighlightTopRight;
inline Sprite ThumbnailHighlightBottomLeft;
inline Sprite ThumbnailHighlightBottomRight;
inline glm::vec2 ThumbnailGridFirstPosition;
inline glm::vec2 ThumbnailGridMargin;
inline RectF ThumbnailGridBounds;
inline float ArrowsAnimationDuration;
inline Sprite ArrowUp;
inline glm::vec2 ArrowUpPosition;
inline Sprite ArrowDown;
inline glm::vec2 ArrowDownPosition;
inline glm::vec2 ThumbnailButtonBorderOffset;
inline float ThumbnailButtonTextFontSize;
inline int ThumbnailButtonTextColorIndex;
inline glm::vec2 ThumbnailButtonTextOffset;

inline float FadeInDuration;
inline float FadeOutDuration;

}  // namespace AlbumMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto