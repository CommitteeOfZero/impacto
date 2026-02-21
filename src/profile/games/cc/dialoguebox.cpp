#include "dialoguebox.h"

#include "../../profile_internal.h"
#include "../../../hud/cc/nametagdisplay.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace DialogueBox {

void Configure() {
  ADVBoxMask = EnsureGetMember<Sprite>("ADVBoxMask");
  ADVBoxEffectDuration = EnsureGetMember<float>("ADVBoxEffectDuration");
}

void ConfigureNametag() {
  NametagMainSprites = GetMemberVector<Sprite>("NametagMainSprites");
  NametagLabelSprites = GetMemberVector<Sprite>("NametagLabelSprites");
  if (NametagMainSprites.size() != NametagLabelSprites.size()) {
    throw std::runtime_error(
        "Number of nametag main sprites not equal to number of "
        "nametag label sprites");
  }

  NametagMainPos = EnsureGetMember<glm::vec2>("NametagMainPos");
  NametagLabelPos = EnsureGetMember<glm::vec2>("NametagLabelPos");

  NametagShowDuration = EnsureGetMember<float>("NametagShowDuration");
}

}  // namespace DialogueBox
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto