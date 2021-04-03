#include "dialoguebox.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace DialogueBox {

Sprite ADVBoxPartLeft;
Sprite ADVBoxPartRight;
Sprite ADVBoxDecoration;

glm::vec2 ADVBoxPartLeftPos;
glm::vec2 ADVBoxPartRightPos;
glm::vec2 ADVBoxDecorationPos;

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