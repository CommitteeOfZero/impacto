#include "dialoguebox.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace DialogueBox {

void Configure() {
  ADVBoxPartLeft = EnsureGetMemberSprite("ADVBoxPartLeft");
  ADVBoxPartRight = EnsureGetMemberSprite("ADVBoxPartRight");
  ADVBoxDecoration = EnsureGetMemberSprite("ADVBoxDecoration");

  ADVBoxPartLeftPos = EnsureGetMemberVec2("ADVBoxPartLeftPos");
  ADVBoxPartRightPos = EnsureGetMemberVec2("ADVBoxPartRightPos");
  ADVBoxDecorationPos = EnsureGetMemberVec2("ADVBoxDecorationPos");
}

}  // namespace DialogueBox
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto