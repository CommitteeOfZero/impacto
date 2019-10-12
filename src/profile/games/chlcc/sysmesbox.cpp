#include "sysmesbox.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SysMesBox {

Sprite Box;
Sprite BoxDecoration;

float BoxX;
float BoxY;

Impacto::SysMesBox::SysMesBoxBase* Configure() {
  Impacto::CHLCC::SysMesBox* result = new Impacto::CHLCC::SysMesBox();

  Box = EnsureGetMemberSprite("Box");
  BoxDecoration = EnsureGetMemberSprite("BoxDecoration");

  BoxX = EnsureGetMemberFloat("BoxX");
  BoxY = EnsureGetMemberFloat("BoxY");

  return result;
}

}  // namespace SysMesBox
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto