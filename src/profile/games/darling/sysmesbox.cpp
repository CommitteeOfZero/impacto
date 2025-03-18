#include "sysmesbox.h"
#include "../../profile_internal.h"

#include "../../../ui/ui.h"
#include "../../../games/darling/sysmesbox.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace Darling {
namespace SysMesBox {

void Configure() {
  SelectionHighlight = EnsureGetMember<Sprite>("SelectionHighlight");

  BoxPartLeft = EnsureGetMember<Sprite>("BoxPartLeft");
  BoxPartRight = EnsureGetMember<Sprite>("BoxPartRight");
  BoxPartMiddle = EnsureGetMember<Sprite>("BoxPartMiddle");
  BoxDecoration = EnsureGetMember<Sprite>("BoxDecoration");

  BoxX = EnsureGetMember<float>("BoxX");
  BoxY = EnsureGetMember<float>("BoxY");
  ChoicePadding = EnsureGetMember<float>("ChoicePadding");
  ChoiceY = EnsureGetMember<float>("ChoiceY");
  ChoiceXBase = EnsureGetMember<float>("ChoiceXBase");
  MinMaxMesWidth = EnsureGetMember<float>("MinMaxMesWidth");
  BoxMinimumWidth = EnsureGetMember<float>("BoxMinimumWidth");
  BoxMiddleBaseX = EnsureGetMember<float>("BoxMiddleBaseX");
  BoxMiddleBaseWidth = EnsureGetMember<float>("BoxMiddleBaseWidth");
  BoxRightBaseWidth = EnsureGetMember<float>("BoxRightBaseWidth");
  BoxRightRemainPad = EnsureGetMember<float>("BoxRightRemainPad");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::SysMesBoxPtr = new UI::Darling::SysMesBox();
  UI::Menus[drawType].push_back(UI::SysMesBoxPtr);
}

}  // namespace SysMesBox
}  // namespace Darling
}  // namespace Profile
}  // namespace Impacto