#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TipsMenu {

int constexpr ThumbnailCount = 37;
int constexpr MaxCategoryString = 5;

inline Sprite TipThumbnails[ThumbnailCount];
inline Sprite TipTextOnlyThumbnail;
inline glm::vec2 ThumbnailPosition;
inline uint16_t CategoryString[MaxCategoryString];

inline int DefaultColorIndex;
inline int UnreadColorIndex;
inline RectF NameInitialBounds;
inline float NameFontSize;
inline RectF PronounciationInitialBounds;
inline float PronounciationFontSize;
inline RectF CategoryInitialBounds;
inline float CategoryEndX;
inline float CategoryFontSize;
inline int SortStringTable;
inline int SortStringIndex;
inline float TipListInitialY;
inline int TipListCategoriesPerPage;
inline int TipListMaxPages;
inline RectF TipListEntryBounds;
inline float TipListEntryFontSize;
inline float TipListYPadding;
inline glm::vec2 TipListEntryHighlightOffset;
inline float TipListEntryNameXOffset;
inline std::string TipListEntryNewText;
inline float TipListEntryNewOffset;
inline int TipListEntryLockedTable;
inline int TipListEntryLockedIndex;
inline int NumberLabelStrTable;
inline int NumberLabelStrIndex;
inline glm::vec2 NumberLabelPosition;
inline float NumberLabelFontSize;
inline RectF NumberBounds;
inline float NumberFontSize;
inline int PageSeparatorTable;
inline int PageSeparatorIndex;
inline glm::vec2 PageSeparatorPosition;
inline float PageSeparatorFontSize;
inline RectF CurrentPageBounds;
inline RectF TotalPagesBounds;

void Configure();

}  // namespace TipsMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto