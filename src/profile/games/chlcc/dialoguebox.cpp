#include "dialoguebox.h"

#include "../../profile_internal.h"
#include "../../dialogue.h"
#include "../../../hud/chlcc/nametagdisplay.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace DialogueBox {

void Configure() {
  SecondaryADVBoxSprite = EnsureGetMember<Sprite>("SecondaryADVBoxSprite");

  ErinBoxSprite = EnsureGetMember<Sprite>("ErinBoxSprite");
  ErinBoxPos = EnsureGetMember<glm::vec2>("ErinBoxPos");
  REVWaitIconOffset = EnsureGetMember<glm::vec2>("REVWaitIconOffset");
}

void ConfigureNametag() {
  Dialogue::NametagPosition = EnsureGetMember<glm::vec2>("NametagPosition");
  Dialogue::NametagSprite = EnsureGetMember<Sprite>("NametagSprite");
  SecondaryNametagSprite = EnsureGetMember<Sprite>("SecondaryNametagSprite");
}

}  // namespace DialogueBox
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto