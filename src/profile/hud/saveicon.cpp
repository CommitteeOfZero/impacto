#include "saveicon.h"
#include "../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace SaveIcon {

void Configure() {
  EnsurePushMemberOfType("SaveIcon", LUA_TTABLE);

  SaveIconCurrentType = SaveIconType::_from_integral_unchecked(
      TryGetMember<int>("SaveIconCurrentType").value_or(SaveIconType::Default));
  DefaultPosition = EnsureGetMember<glm::vec2>("DefaultPosition");
  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

  switch (SaveIconCurrentType) {
    default:
    case +SaveIconType::Default: {
      ForegroundAnimation =
          EnsureGetMember<SpriteAnimationDef>("ForegroundAnimation");
      BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
      BackgroundOffset = EnsureGetMember<glm::vec2>("BackgroundOffset");
      BackgroundMaxAlpha = EnsureGetMember<float>("BackgroundMaxAlpha");
      break;
    }
    case +SaveIconType::CHLCC: {
      SaveIconSprites = GetMemberVector<Sprite>("SaveIconSprites");
      if (SaveIconSprites.size() != 3) {
        throw std::runtime_error("Wrong number of sprites for CHLCC SaveIcon");
      }
      ActiveAnimationDuration =
          EnsureGetMember<float>("ActiveAnimationDuration");
      break;
    }
  }

  Pop();
}

}  // namespace SaveIcon
}  // namespace Profile
}  // namespace Impacto