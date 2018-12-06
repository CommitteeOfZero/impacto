#include "saveicon.h"
#include "../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace SaveIcon {

SpriteAnimationDef ForegroundAnimation;
glm::vec2 DefaultPosition;
Sprite BackgroundSprite;
glm::vec2 BackgroundOffset;
float BackgroundMaxAlpha;
float FadeInDuration;
float FadeOutDuration;

void Configure() {
  EnsurePushMemberOfType("SaveIcon", kObjectType);

  ForegroundAnimation = EnsureGetMemberAnimation("ForegroundAnimation");
  DefaultPosition = EnsureGetMemberVec2("DefaultPosition");
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  BackgroundOffset = EnsureGetMemberVec2("BackgroundOffset");
  BackgroundMaxAlpha = EnsureGetMemberFloat("BackgroundMaxAlpha");
  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

  Pop();
}

}  // namespace SaveIcon
}  // namespace Profile
}  // namespace Impacto