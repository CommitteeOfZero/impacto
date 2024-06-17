#include "saveicon.h"
#include "../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace SaveIcon {

void Configure() {
  EnsurePushMemberOfType("SaveIcon", LUA_TTABLE);

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