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
  Box = EnsureGetMember<Sprite>("Box");
  BoxDecoration = EnsureGetMember<Sprite>("BoxDecoration");
  SelectionLeftPart = EnsureGetMember<Sprite>("SelectionLeftPart");
  SelectionRightPart = EnsureGetMember<Sprite>("SelectionRightPart");
  SelectionMiddlePart = EnsureGetMember<Sprite>("SelectionMiddlePart");
  LoadingStar = EnsureGetMember<Sprite>("LoadingStar");

  BoxX = EnsureGetMember<float>("BoxX");
  BoxY = EnsureGetMember<float>("BoxY");
  ChoicePadding = EnsureGetMember<float>("ChoicePadding");
  ChoiceY = EnsureGetMember<float>("ChoiceY");
  ChoiceXBase = EnsureGetMember<float>("ChoiceXBase");
  MinMaxMesWidth = EnsureGetMember<float>("MinMaxMesWidth");
  MinHighlightWidth = EnsureGetMember<float>("MinHighlightWidth");
  HighlightBaseWidth = EnsureGetMember<float>("HighlightBaseWidth");
  HighlightRightPartSpriteWidth =
      EnsureGetMember<float>("HighlightRightPartSpriteWidth");
  HighlightYOffset = EnsureGetMember<float>("HighlightYOffset");
  HighlightXOffset = EnsureGetMember<float>("HighlightXOffset");
  HighlightXBase = EnsureGetMember<float>("HighlightXBase");
  HighlightXStep = EnsureGetMember<float>("HighlightXStep");
  LoadingStarsFadeDuration = EnsureGetMember<float>("LoadingStarsFadeDuration");
  LoadingStarsPosition = EnsureGetMember<glm::vec2>("LoadingStarsPosition");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::SysMesBoxPtr = new UI::CHLCC::SysMesBox();
  UI::Menus[drawType].push_back(UI::SysMesBoxPtr);
}

}  // namespace SysMesBox
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto