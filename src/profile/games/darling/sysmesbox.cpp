#include "sysmesbox.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace Darling {
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
float BoxRightBaseWidth;
float BoxRightRemainPad;

void Configure() {
  BoxPartLeft = EnsureGetMemberSprite("BoxPartLeft");
  BoxPartRight = EnsureGetMemberSprite("BoxPartRight");
  BoxPartMiddle = EnsureGetMemberSprite("BoxPartMiddle");
  BoxDecoration = EnsureGetMemberSprite("BoxDecoration");

  BoxX = EnsureGetMemberFloat("BoxX");
  BoxY = EnsureGetMemberFloat("BoxY");
  BoxMinimumWidth = EnsureGetMemberFloat("BoxMinimumWidth");
  BoxMiddleBaseX = EnsureGetMemberFloat("BoxMiddleBaseX");
  BoxMiddleBaseWidth = EnsureGetMemberFloat("BoxMiddleBaseWidth");
  BoxRightBaseWidth = EnsureGetMemberFloat("BoxRightBaseWidth");
  BoxRightRemainPad = EnsureGetMemberFloat("BoxRightRemainPad");
}

}  // namespace SysMesBox
}  // namespace Darling
}  // namespace Profile
}  // namespace Impacto