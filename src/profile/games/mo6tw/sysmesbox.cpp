#include "sysmesbox.h"
#include "../../profile_internal.h"

#include "../../../ui/ui.h"
#include "../../../games/mo6tw/sysmesbox.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace SysMesBox {

void Configure() {
  SelectionHighlight = EnsureGetMemberSprite("SelectionHighlight");

  BoxPartLeft = EnsureGetMemberSprite("BoxPartLeft");
  BoxPartRight = EnsureGetMemberSprite("BoxPartRight");
  BoxPartMiddle = EnsureGetMemberSprite("BoxPartMiddle");
  BoxDecoration = EnsureGetMemberSprite("BoxDecoration");

  BoxX = EnsureGetMemberFloat("BoxX");
  BoxY = EnsureGetMemberFloat("BoxY");
  ChoicePadding = EnsureGetMemberFloat("ChoicePadding");
  ChoiceY = EnsureGetMemberFloat("ChoiceY");
  ChoiceXBase = EnsureGetMemberFloat("ChoiceXBase");
  MinMaxMesWidth = EnsureGetMemberFloat("MinMaxMesWidth");
  BoxMinimumWidth = EnsureGetMemberFloat("BoxMinimumWidth");
  BoxMiddleBaseX = EnsureGetMemberFloat("BoxMiddleBaseX");
  BoxMiddleBaseWidth = EnsureGetMemberFloat("BoxMiddleBaseWidth");
  BoxMiddleRemainBase = EnsureGetMemberFloat("BoxMiddleRemainBase");
  BoxRightBaseX = EnsureGetMemberFloat("BoxRightBaseX");
  BoxRightRemainPad = EnsureGetMemberFloat("BoxRightRemainPad");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SysMesBoxPtr = new UI::MO6TW::SysMesBox();
  UI::Menus[drawType].push_back(UI::SysMesBoxPtr);
}

}  // namespace SysMesBox
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto