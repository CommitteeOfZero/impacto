#include "sysmesbox.h"
#include "../../profile_internal.h"

#include "../../../ui/ui.h"
#include "../../../games/chlcc/sysmesbox.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SysMesBox {

void Configure() {
  Box = EnsureGetMemberSprite("Box");
  BoxDecoration = EnsureGetMemberSprite("BoxDecoration");
  SelectionLeftPart = EnsureGetMemberSprite("SelectionLeftPart");
  SelectionRightPart = EnsureGetMemberSprite("SelectionRightPart");
  SelectionMiddlePart = EnsureGetMemberSprite("SelectionMiddlePart");

  BoxX = EnsureGetMemberFloat("BoxX");
  BoxY = EnsureGetMemberFloat("BoxY");
  ChoicePadding = EnsureGetMemberFloat("ChoicePadding");
  ChoiceY = EnsureGetMemberFloat("ChoiceY");
  ChoiceXBase = EnsureGetMemberFloat("ChoiceXBase");
  MinMaxMesWidth = EnsureGetMemberFloat("MinMaxMesWidth");
  MinHighlightWidth = EnsureGetMemberFloat("MinHighlightWidth");
  HighlightBaseWidth = EnsureGetMemberFloat("HighlightBaseWidth");
  HighlightRightPartSpriteWidth =
      EnsureGetMemberFloat("HighlightRightPartSpriteWidth");
  HighlightYOffset = EnsureGetMemberFloat("HighlightYOffset");
  HighlightXOffset = EnsureGetMemberFloat("HighlightXOffset");
  HighlightXBase = EnsureGetMemberFloat("HighlightXBase");
  HighlightXStep = EnsureGetMemberFloat("HighlightXStep");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SysMesBoxPtr = new UI::CHLCC::SysMesBox();
  UI::Menus[drawType].push_back(UI::SysMesBoxPtr);
}

}  // namespace SysMesBox
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto