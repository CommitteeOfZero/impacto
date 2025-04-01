#include "tipsmenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../text.h"

#include "../../ui/tipsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/cclcc/tipsmenu.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TipsMenu {

void Configure() {
  TipsGuideSprite = EnsureGetMember<Sprite>("TipsGuideSprite");
  TipsMaskSprite = EnsureGetMember<Sprite>("TipsMaskSprite");
  TipsHighlightedSprite = EnsureGetMember<Sprite>("TipsHighlightedSprite");
  TipsHighlightedTabSprite =
      EnsureGetMember<Sprite>("TipsHighlightedTabSprite");
  TipsNewSprite = EnsureGetMember<Sprite>("TipsNewSprite");
  TipsHighlightedTabAdder = EnsureGetMember<int>("TipsHighlightedTabAdder");

  TipsGuideX = EnsureGetMember<int>("TipsGuideX");
  TipsGuideY = EnsureGetMember<int>("TipsGuideY");

  TipsTextTableIndex = EnsureGetMember<int>("TipsTextTableIndex");
  TipsTextSortStringIndex = EnsureGetMember<int>("TipsTextSortStringIndex");
  TipsTextEntryLockedIndex = EnsureGetMember<int>("TipsTextEntryLockedIndex");

  TipsEntryBounds = EnsureGetMember<RectF>("TipsEntryBounds");
  TipEntryNewOffset = EnsureGetMember<glm::vec2>("TipEntryNewOffset");
  TipsEntryHighlightOffset =
      EnsureGetMember<glm::vec2>("TipsEntryHighlightOffset");
  TipsEntryNumberOffset = EnsureGetMember<glm::vec2>("TipsEntryNumberOffset");
  TipsEntryNameOffset = EnsureGetMember<glm::vec2>("TipsEntryNameOffset");

  TipsTabBounds = EnsureGetMember<RectF>("TipsTabBounds");
  TipsTabNameDisplay = EnsureGetMember<glm::vec2>("TipsTabNameDisplay");

  CategoryPos = EnsureGetMember<glm::vec2>("CategoryPos");
  CategoryFontSize = EnsureGetMember<int>("CategoryFontSize");
  NamePos = EnsureGetMember<glm::vec2>("NamePos");
  NameFontSize = EnsureGetMember<int>("NameFontSize");
  PronounciationPos = EnsureGetMember<glm::vec2>("PronounciationPos");
  PronounciationFontSize = EnsureGetMember<int>("PronounciationFontSize");
  NumberPos = EnsureGetMember<glm::vec2>("NumberPos");
  NumberFontSize = EnsureGetMember<int>("NumberFontSize");

  TipsEntryNameUnreadColor =
      EnsureGetMember<uint32_t>("TipsEntryNameUnreadColor");
  TipsMenuDarkTextColor = EnsureGetMember<uint32_t>("TipsMenuDarkTextColor");

  TipsScrollThumbSprite = EnsureGetMember<Sprite>("TipsScrollThumbSprite");
  TipsScrollThumbLength = EnsureGetMember<float>("TipsScrollThumbLength");
  TipsScrollYStart = EnsureGetMember<int>("TipsScrollYStart");
  TipsScrollYEnd = EnsureGetMember<int>("TipsScrollYEnd");
  TipsScrollEntriesX = EnsureGetMember<int>("TipsScrollEntriesX");
  TipsScrollDetailsX = EnsureGetMember<int>("TipsScrollDetailsX");

  TipsMaskSheet = EnsureGetMember<SpriteSheet>("TipsMask");

  TransitionInDuration = EnsureGetMember<float>("TransitionInDuration");
  TransitionOutDuration = EnsureGetMember<float>("TransitionOutDuration");

  MinHoldTime = EnsureGetMember<float>("MinHoldTime");
  AdvanceFocusTimeInterval = EnsureGetMember<float>("AdvanceFocusTimeInterval");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::TipsMenuPtr = new UI::CCLCC::TipsMenu();
  UI::Menus[drawType].push_back(UI::TipsMenuPtr);
}

}  // namespace TipsMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto
