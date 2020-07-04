#include "sysmesbox.h"
#include "../../profile_internal.h"

#include "../../../ui/ui.h"
#include "../../../games/mo6tw/sysmesbox.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
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
float BoxMiddleRemainBase;
float BoxRightBaseX;
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
  BoxMiddleRemainBase = EnsureGetMemberFloat("BoxMiddleRemainBase");
  BoxRightBaseX = EnsureGetMemberFloat("BoxRightBaseX");
  BoxRightRemainPad = EnsureGetMemberFloat("BoxRightRemainPad");

  UI::SysMesBoxPtr = new UI::MO6TW::SysMesBox();
}

}  // namespace SysMesBox
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto