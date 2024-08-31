#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TipsMenu {

inline Sprite TipsBookLayerSprite;
inline Sprite TipsGuideSprite;
inline Sprite TipsMaskSprite;
inline Sprite TipsHighlightedSprite;
inline Sprite TipsNewSprite;

inline int TipsGuideX;
inline int TipsGuideY;

inline int TipsTextTableIndex;
inline int TipsTextSortStringIndex;
inline int TipsTextEntryLockedIndex;

inline glm::vec2 TipEntryNewOffset;
inline glm::vec2 TipsEntryNameInitDisplay;
inline glm::vec2 TipsEntryNumberOffset;
inline glm::vec2 TipsEntryHighlightOffset;
inline int TipsEntryNameOffset;
inline RectF TipsTabBounds;

inline Sprite TipsHighlightedTabSprite;
inline glm::vec2 TipsTabNameDisplay;
inline int TipsHighlightedTabAdder;

void Configure();

}  // namespace TipsMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto