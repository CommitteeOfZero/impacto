#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TipsMenu {

int const ThumbnailCount = 37;
int const MaxCategoryString = 5;

extern Sprite TipThumbnails[ThumbnailCount];
extern Sprite TipTextOnlyThumbnail;
extern glm::vec2 ThumbnailPosition;
extern uint16_t CategoryString[MaxCategoryString];

extern int DefaultColorIndex;
extern int UnreadColorIndex;
extern RectF NameInitialBounds;
extern float NameFontSize;
extern RectF PronounciationInitialBounds;
extern float PronounciationFontSize;
extern RectF CategoryInitialBounds;
extern float CategoryEndX;
extern float CategoryFontSize;
extern int SortStringTable;
extern int SortStringIndex;
extern float TipListInitialY;
extern int TipListCategoriesPerPage;
extern int TipListMaxPages;
extern RectF TipListEntryBounds;
extern float TipListEntryFontSize;
extern float TipListYPadding;
extern glm::vec2 TipListEntryHighlightOffset;
extern float TipListEntryNameXOffset;
extern std::string TipListEntryNewText;
extern float TipListEntryNewOffset;
extern int TipListEntryLockedTable;
extern int TipListEntryLockedIndex;
extern int NumberLabelStrTable;
extern int NumberLabelStrIndex;
extern glm::vec2 NumberLabelPosition;
extern float NumberLabelFontSize;
extern RectF NumberBounds;
extern float NumberFontSize;
extern int PageSeparatorTable;
extern int PageSeparatorIndex;
extern glm::vec2 PageSeparatorPosition;
extern float PageSeparatorFontSize;
extern RectF CurrentPageBounds;
extern RectF TotalPagesBounds;

void Configure();

}  // namespace TipsMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto