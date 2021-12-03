#include "sysmesbox.h"
#include "../../profile_internal.h"

#include "../../../ui/ui.h"
#include "../../../games/darling/sysmesbox.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace Darling {
namespace SysMesBox {

Sprite BoxPartLeft;
Sprite BoxPartRight;
Sprite BoxPartMiddle;
Sprite BoxDecoration;
Sprite SelectionHighlight;

float BoxX;
float BoxY;
float ChoicePadding;
float ChoiceY;
float ChoiceXBase;
float MinMaxMesWidth;
float BoxMinimumWidth;
float BoxMiddleBaseX;
float BoxMiddleBaseWidth;
float BoxRightBaseWidth;
float BoxRightRemainPad;

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
  BoxRightBaseWidth = EnsureGetMemberFloat("BoxRightBaseWidth");
  BoxRightRemainPad = EnsureGetMemberFloat("BoxRightRemainPad");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::SysMesBoxPtr = new UI::Darling::SysMesBox();
  UI::Menus[drawType].push_back(UI::SysMesBoxPtr);
}

}  // namespace SysMesBox
}  // namespace Darling
}  // namespace Profile
}  // namespace Impacto