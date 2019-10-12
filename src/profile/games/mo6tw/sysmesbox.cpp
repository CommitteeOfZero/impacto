#include "sysmesbox.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace SysMesBox {

Sprite BoxPartLeft;
Sprite BoxPartRight;
Sprite BoxPartMiddle;
Sprite BoxDecoration;

float BoxX;
float BoxY;
float BoxMinimumWidth;
float BoxMiddleBaseX;
float BoxMiddleBaseWidth;
float BoxMiddleRemainBase;
float BoxRightBaseX;
float BoxRightRemainPad;

Impacto::SysMesBox::SysMesBoxBase* Configure() {
  Impacto::MO6TW::SysMesBox* result = new Impacto::MO6TW::SysMesBox();

  BoxPartLeft = EnsureGetMemberSprite("BoxPartLeft");
  BoxPartRight = EnsureGetMemberSprite("BoxPartRight");
  BoxPartMiddle = EnsureGetMemberSprite("BoxPartMiddle");
  BoxDecoration = EnsureGetMemberSprite("BoxDecoration");

  BoxX = EnsureGetMemberFloat("BoxX");
  BoxY = EnsureGetMemberFloat("BoxY");
  BoxMinimumWidth = EnsureGetMemberFloat("BoxMinimumWidth");
  BoxMiddleBaseX = EnsureGetMemberFloat("BoxMiddleBaseX");
  BoxMiddleBaseWidth = EnsureGetMemberFloat("BoxMiddleBaseWidth");
  BoxMiddleRemainBase = EnsureGetMemberFloat("BoxMiddleRemainBase");
  BoxRightBaseX = EnsureGetMemberFloat("BoxRightBaseX");
  BoxRightRemainPad = EnsureGetMemberFloat("BoxRightRemainPad");

  return result;
}

}  // namespace SysMesBox
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto