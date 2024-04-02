#include "tipsmenu.h"
#include "../../profile_internal.h"
#include "../../../renderer/renderer.h"
#include "../../../log.h"
#include "../../../text.h"

#include "../../ui/tipsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/mo6tw/tipsmenu.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TipsMenu {

Sprite TipThumbnails[ThumbnailCount];
Sprite TipTextOnlyThumbnail;
glm::vec2 ThumbnailPosition;
uint16_t CategoryString[MaxCategoryString];

int DefaultColorIndex;
int UnreadColorIndex;
RectF NameInitialBounds;
float NameFontSize;
RectF PronounciationInitialBounds;
float PronounciationFontSize;
RectF CategoryInitialBounds;
float CategoryEndX;
float CategoryFontSize;
int SortStringTable;
int SortStringIndex;
float TipListInitialY;
int TipListCategoriesPerPage;
int TipListMaxPages;
RectF TipListEntryBounds;
float TipListEntryFontSize;
float TipListYPadding;
glm::vec2 TipListEntryHighlightOffset;
float TipListEntryNameXOffset;
std::string TipListEntryNewText;
float TipListEntryNewOffset;
int TipListEntryLockedTable;
int TipListEntryLockedIndex;
int NumberLabelStrTable;
int NumberLabelStrIndex;
glm::vec2 NumberLabelPosition;
float NumberLabelFontSize;
RectF NumberBounds;
float NumberFontSize;
int PageSeparatorTable;
int PageSeparatorIndex;
glm::vec2 PageSeparatorPosition;
float PageSeparatorFontSize;
RectF CurrentPageBounds;
RectF TotalPagesBounds;

void Configure() {
  GetMemberSpriteArray(TipThumbnails, 37, "Thumbnails");
  TipTextOnlyThumbnail = EnsureGetMemberSprite("TextOnlyThumbnail");
  ThumbnailPosition = EnsureGetMemberVec2("ThumbnailPosition");
  auto str = EnsureGetMemberString("CategoryString");
  TextGetSc3String(str, CategoryString);

  DefaultColorIndex = EnsureGetMemberInt("DefaultColorIndex");
  UnreadColorIndex = EnsureGetMemberInt("UnreadColorIndex");
  NameInitialBounds = EnsureGetMemberRectF("NameInitialBounds");
  NameFontSize = EnsureGetMemberFloat("NameFontSize");
  PronounciationInitialBounds =
      EnsureGetMemberRectF("PronounciationInitialBounds");
  PronounciationFontSize = EnsureGetMemberFloat("PronounciationFontSize");
  CategoryInitialBounds = EnsureGetMemberRectF("CategoryInitialBounds");
  CategoryEndX = EnsureGetMemberFloat("CategoryEndX");
  CategoryFontSize = EnsureGetMemberFloat("CategoryFontSize");
  SortStringTable = EnsureGetMemberInt("SortStringTable");
  SortStringIndex = EnsureGetMemberInt("SortStringIndex");
  TipListInitialY = EnsureGetMemberFloat("TipListInitialY");
  TipListCategoriesPerPage = EnsureGetMemberInt("TipListCategoriesPerPage");
  TipListMaxPages = EnsureGetMemberInt("TipListMaxPages");
  TipListEntryBounds = EnsureGetMemberRectF("TipListEntryBounds");
  TipListEntryFontSize = EnsureGetMemberFloat("TipListEntryFontSize");
  TipListYPadding = EnsureGetMemberFloat("TipListYPadding");
  TipListEntryHighlightOffset =
      EnsureGetMemberVec2("TipListEntryHighlightOffset");
  TipListEntryNameXOffset = EnsureGetMemberFloat("TipListEntryNameXOffset");
  TipListEntryNewText = EnsureGetMemberString("TipListEntryNewText");
  TipListEntryNewOffset = EnsureGetMemberFloat("TipListEntryNewOffset");
  TipListEntryLockedTable = EnsureGetMemberInt("TipListEntryLockedTable");
  TipListEntryLockedIndex = EnsureGetMemberInt("TipListEntryLockedIndex");
  NumberLabelStrTable = EnsureGetMemberInt("NumberLabelStrTable");
  NumberLabelStrIndex = EnsureGetMemberInt("NumberLabelStrIndex");
  NumberLabelPosition = EnsureGetMemberVec2("NumberLabelPosition");
  NumberLabelFontSize = EnsureGetMemberFloat("NumberLabelFontSize");
  NumberBounds = EnsureGetMemberRectF("NumberBounds");
  NumberFontSize = EnsureGetMemberFloat("NumberFontSize");
  PageSeparatorTable = EnsureGetMemberInt("PageSeparatorTable");
  PageSeparatorIndex = EnsureGetMemberInt("PageSeparatorIndex");
  PageSeparatorPosition = EnsureGetMemberVec2("PageSeparatorPosition");
  PageSeparatorFontSize = EnsureGetMemberFloat("PageSeparatorFontSize");
  CurrentPageBounds = EnsureGetMemberRectF("CurrentPageBounds");
  TotalPagesBounds = EnsureGetMemberRectF("TotalPagesBounds");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::TipsMenuPtr = new UI::MO6TW::TipsMenu();
  UI::Menus[drawType].push_back(UI::TipsMenuPtr);
}

}  // namespace TipsMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
