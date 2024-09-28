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
  TipsGuideSprite = EnsureGetMemberSprite("TipsGuideSprite");
  TipsMaskSprite = EnsureGetMemberSprite("TipsMaskSprite");
  TipsHighlightedSprite = EnsureGetMemberSprite("TipsHighlightedSprite");
  TipsHighlightedTabSprite = EnsureGetMemberSprite("TipsHighlightedTabSprite");
  TipsNewSprite = EnsureGetMemberSprite("TipsNewSprite");
  TipsHighlightedTabAdder = EnsureGetMemberInt("TipsHighlightedTabAdder");

  TipsGuideX = EnsureGetMemberInt("TipsGuideX");
  TipsGuideY = EnsureGetMemberInt("TipsGuideY");

  TipsTextTableIndex = EnsureGetMemberInt("TipsTextTableIndex");
  TipsTextSortStringIndex = EnsureGetMemberInt("TipsTextSortStringIndex");
  TipsTextEntryLockedIndex = EnsureGetMemberInt("TipsTextEntryLockedIndex");

  TipsEntryBounds = EnsureGetMemberRectF("TipsEntryBounds");
  TipEntryNewOffset = EnsureGetMemberVec2("TipEntryNewOffset");
  TipsEntryHighlightOffset = EnsureGetMemberVec2("TipsEntryHighlightOffset");
  TipsEntryNumberOffset = EnsureGetMemberVec2("TipsEntryNumberOffset");
  TipsEntryNameOffset = EnsureGetMemberVec2("TipsEntryNameOffset");

  TipsTabBounds = EnsureGetMemberRectF("TipsTabBounds");
  TipsTabNameDisplay = EnsureGetMemberVec2("TipsTabNameDisplay");

  CategoryPos = EnsureGetMemberVec2("CategoryPos");
  CategoryFontSize = EnsureGetMemberInt("CategoryFontSize");
  NamePos = EnsureGetMemberVec2("NamePos");
  NameFontSize = EnsureGetMemberInt("NameFontSize");
  PronounciationPos = EnsureGetMemberVec2("PronounciationPos");
  PronounciationFontSize = EnsureGetMemberInt("PronounciationFontSize");
  NumberPos = EnsureGetMemberVec2("NumberPos");
  NumberFontSize = EnsureGetMemberInt("NumberFontSize");

  TipsEntryNameUnreadColor = EnsureGetMemberUint("TipsEntryNameUnreadColor");
  TipsMenuDarkTextColor = EnsureGetMemberUint("TipsMenuDarkTextColor");

  TipsScrollThumbSprite = EnsureGetMemberSprite("TipsScrollThumbSprite");
  TipsScrollThumbLength = EnsureGetMemberFloat("TipsScrollThumbLength");
  TipsScrollYStart = EnsureGetMemberInt("TipsScrollYStart");
  TipsScrollYEnd = EnsureGetMemberInt("TipsScrollYEnd");
  TipsScrollEntriesX = EnsureGetMemberInt("TipsScrollEntriesX");
  TipsScrollDetailsX = EnsureGetMemberInt("TipsScrollDetailsX");

  TipsMaskSheet = EnsureGetMemberSpriteSheet("TipsMask");

  TransitionInDuration = EnsureGetMemberFloat("TransitionInDuration");
  TransitionOutDuration = EnsureGetMemberFloat("TransitionOutDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::TipsMenuPtr = new UI::CCLCC::TipsMenu();
  UI::Menus[drawType].push_back(UI::TipsMenuPtr);
}

}  // namespace TipsMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto
