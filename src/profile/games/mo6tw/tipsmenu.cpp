#include "tipsmenu.h"
#include "../../profile_internal.h"
#include "../../../renderer/renderer.h"
#include "../../../log.h"
#include "../../../text/text.h"

#include "../../ui/tipsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/mo6tw/tipsmenu.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TipsMenu {

void Configure() {
  GetMemberArray<Sprite>(TipThumbnails, 37, "Thumbnails");
  TipTextOnlyThumbnail = EnsureGetMember<Sprite>("TextOnlyThumbnail");
  ThumbnailPosition = EnsureGetMember<glm::vec2>("ThumbnailPosition");
  auto str = EnsureGetMember<char const*>("CategoryString");
  TextGetSc3String(str, CategoryString);

  DefaultColorIndex = EnsureGetMember<int>("DefaultColorIndex");
  UnreadColorIndex = EnsureGetMember<int>("UnreadColorIndex");
  NameInitialBounds = EnsureGetMember<RectF>("NameInitialBounds");
  NameFontSize = EnsureGetMember<float>("NameFontSize");
  PronounciationInitialBounds =
      EnsureGetMember<RectF>("PronounciationInitialBounds");
  PronounciationFontSize = EnsureGetMember<float>("PronounciationFontSize");
  CategoryInitialBounds = EnsureGetMember<RectF>("CategoryInitialBounds");
  CategoryEndX = EnsureGetMember<float>("CategoryEndX");
  CategoryFontSize = EnsureGetMember<float>("CategoryFontSize");
  SortStringTable = EnsureGetMember<int>("SortStringTable");
  SortStringIndex = EnsureGetMember<int>("SortStringIndex");
  TipListInitialY = EnsureGetMember<float>("TipListInitialY");
  TipListCategoriesPerPage = EnsureGetMember<int>("TipListCategoriesPerPage");
  TipListMaxPages = EnsureGetMember<int>("TipListMaxPages");
  TipListEntryBounds = EnsureGetMember<RectF>("TipListEntryBounds");
  TipListEntryFontSize = EnsureGetMember<float>("TipListEntryFontSize");
  TipListYPadding = EnsureGetMember<float>("TipListYPadding");
  TipListEntryHighlightOffset =
      EnsureGetMember<glm::vec2>("TipListEntryHighlightOffset");
  TipListEntryNameXOffset = EnsureGetMember<float>("TipListEntryNameXOffset");
  TipListEntryNewText = EnsureGetMember<std::string>("TipListEntryNewText");
  TipListEntryNewOffset = EnsureGetMember<float>("TipListEntryNewOffset");
  TipListEntryLockedTable = EnsureGetMember<int>("TipListEntryLockedTable");
  TipListEntryLockedIndex = EnsureGetMember<int>("TipListEntryLockedIndex");
  NumberLabelStrTable = EnsureGetMember<int>("NumberLabelStrTable");
  NumberLabelStrIndex = EnsureGetMember<int>("NumberLabelStrIndex");
  NumberLabelPosition = EnsureGetMember<glm::vec2>("NumberLabelPosition");
  NumberLabelFontSize = EnsureGetMember<float>("NumberLabelFontSize");
  NumberBounds = EnsureGetMember<RectF>("NumberBounds");
  NumberFontSize = EnsureGetMember<float>("NumberFontSize");
  PageSeparatorTable = EnsureGetMember<int>("PageSeparatorTable");
  PageSeparatorIndex = EnsureGetMember<int>("PageSeparatorIndex");
  PageSeparatorPosition = EnsureGetMember<glm::vec2>("PageSeparatorPosition");
  PageSeparatorFontSize = EnsureGetMember<float>("PageSeparatorFontSize");
  CurrentPageBounds = EnsureGetMember<RectF>("CurrentPageBounds");
  TotalPagesBounds = EnsureGetMember<RectF>("TotalPagesBounds");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::TipsMenuPtr = new UI::MO6TW::TipsMenu();
  UI::Menus[drawType].push_back(UI::TipsMenuPtr);
}

}  // namespace TipsMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto
