#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TipsMenu {

inline Sprite TipsGuideSprite;
inline Sprite TipsMaskSprite;
inline Sprite TipsHighlightedSprite;
inline Sprite TipsNewSprite;

inline int TipsGuideX;
inline int TipsGuideY;

inline int TipsTextTableIndex;
inline int TipsTextSortStringIndex;
inline int TipsTextEntryLockedIndex;

inline RectF TipsEntryBounds;
inline glm::vec2 TipEntryNewOffset;
inline glm::vec2 TipsEntryNameOffset;
inline glm::vec2 TipsEntryNumberOffset;
inline glm::vec2 TipsEntryHighlightOffset;
inline RectF TipsTabBounds;

inline Sprite TipsHighlightedTabSprite;
inline glm::vec2 TipsTabNameDisplay;
inline int TipsHighlightedTabAdder;

inline glm::vec2 CategoryPos;
inline int CategoryFontSize;
inline glm::vec2 NamePos;
inline int NameFontSize;
inline glm::vec2 PronounciationPos;
inline int PronounciationFontSize;
inline glm::vec2 NumberPos;
inline int NumberFontSize;

inline uint32_t TipsEntryNameUnreadColor;
inline uint32_t TipsMenuDarkTextColor;

inline Sprite TipsScrollThumbSprite;
inline int TipsScrollYStart;
inline int TipsScrollYEnd;
inline int TipsScrollEntriesX;
inline int TipsScrollDetailsX;

inline SpriteSheet TipsMaskSheet;

void Configure();

}  // namespace TipsMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto