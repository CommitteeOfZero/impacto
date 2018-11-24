#include "saveicon.h"
#include "profile_internal.h"

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
  auto const& _saveIcon =
      EnsureGetMemberOfType(Json, "/", "SaveIcon", kObjectType);

  ForegroundAnimation =
      EnsureGetMemberAnimation(_saveIcon, "/SaveIcon", "ForegroundAnimation");
  DefaultPosition =
      EnsureGetMemberVec2(_saveIcon, "/SaveIcon", "DefaultPosition");
  BackgroundSprite =
      EnsureGetMemberSprite(_saveIcon, "/SaveIcon", "BackgroundSprite");
  BackgroundOffset =
      EnsureGetMemberVec2(_saveIcon, "/SaveIcon", "BackgroundOffset");
  BackgroundMaxAlpha =
      EnsureGetMemberFloat(_saveIcon, "/SaveIcon", "BackgroundMaxAlpha");
  FadeInDuration =
      EnsureGetMemberFloat(_saveIcon, "/SaveIcon", "FadeInDuration");
  FadeOutDuration =
      EnsureGetMemberFloat(_saveIcon, "/SaveIcon", "FadeOutDuration");
}

}  // namespace SaveIcon
}  // namespace Profile
}  // namespace Impacto