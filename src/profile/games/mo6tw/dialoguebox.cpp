#include "dialoguebox.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace DialogueBox {

void Configure() {
  ADVBoxPartLeft = EnsureGetMember<Sprite>("ADVBoxPartLeft");
  ADVBoxPartRight = EnsureGetMember<Sprite>("ADVBoxPartRight");
  ADVBoxDecoration = EnsureGetMember<Sprite>("ADVBoxDecoration");

  ADVBoxPartLeftPos = EnsureGetMember<glm::vec2>("ADVBoxPartLeftPos");
  ADVBoxPartRightPos = EnsureGetMember<glm::vec2>("ADVBoxPartRightPos");
  ADVBoxDecorationPos = EnsureGetMember<glm::vec2>("ADVBoxDecorationPos");
}

}  // namespace DialogueBox
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto