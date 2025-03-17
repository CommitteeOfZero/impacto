#include "saveicon.h"
#include "../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace SaveIcon {

void Configure() {
  EnsurePushMemberOfType("SaveIcon", LUA_TTABLE);

  ForegroundAnimation =
      EnsureGetMember<SpriteAnimationDef>("ForegroundAnimation");
  DefaultPosition = EnsureGetMember<glm::vec2>("DefaultPosition");
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  BackgroundOffset = EnsureGetMember<glm::vec2>("BackgroundOffset");
  BackgroundMaxAlpha = EnsureGetMember<float>("BackgroundMaxAlpha");
  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

  Pop();
}

}  // namespace SaveIcon
}  // namespace Profile
}  // namespace Impacto