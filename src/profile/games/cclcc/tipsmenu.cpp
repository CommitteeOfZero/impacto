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
  TipsBookLayerSprite = EnsureGetMemberSprite("TipsBookLayerSprite");
  TipsGuideSprite = EnsureGetMemberSprite("TipsGuideSprite");
  TipsMaskSprite = EnsureGetMemberSprite("TipsMaskSprite");
  TipsHighlightedSprite = EnsureGetMemberSprite("TipsHighlightedSprite");
  TipsHighlightedTabSprite = EnsureGetMemberSprite("TipsHighlightedTabSprite");
  TipsNewSprite = EnsureGetMemberSprite("TipsNewSprite");
  TipsHighlightedTabAdder = EnsureGetMemberInt("TipsHighlightedTabAdder");

  TipsGuideX = EnsureGetMemberInt("TipsGuideX");
  TipsGuideY = EnsureGetMemberInt("TipsGuideY");

  TipEntryNewOffset = EnsureGetMemberVec2("TipEntryNewOffset");

  TipsTextTableIndex = EnsureGetMemberInt("TipsTextTableIndex");
  TipsTextSortStringIndex = EnsureGetMemberInt("TipsTextSortStringIndex");
  TipsTextEntryLockedIndex = EnsureGetMemberInt("TipsTextEntryLockedIndex");

  TipsEntryNameInitDisplay = EnsureGetMemberVec2("TipsEntryNameInitDisplay");
  TipsEntryNameOffset = EnsureGetMemberInt("TipsEntryNameOffset");
  TipsEntryNumberOffset = EnsureGetMemberVec2("TipsEntryNumberOffset");
  TipsEntryHighlightOffset = EnsureGetMemberVec2("TipsEntryHighlightOffset");
  TipsTabBounds = EnsureGetMemberRectF("TipsTabBounds");
  TipsTabNameDisplay = EnsureGetMemberVec2("TipsTabNameDisplay");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::TipsMenuPtr = new UI::CCLCC::TipsMenu();
  UI::Menus[drawType].push_back(UI::TipsMenuPtr);
}

}  // namespace TipsMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto
