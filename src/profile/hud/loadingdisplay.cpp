#include "loadingdisplay.h"
#include "../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace LoadingDisplay {

void Configure() {
  EnsurePushMemberOfType("LoadingDisplay", LUA_TTABLE);

  ResourceLoadBgAnim =
      EnsureGetMember<SpriteAnimationDef>("ResourceLoadBgAnim");
  SaveLoadBgAnim = EnsureGetMember<SpriteAnimationDef>("SaveLoadBgAnim");
  LoadingIconAnim = EnsureGetMember<SpriteAnimationDef>("LoadingIconAnim");
  LoadingTextAnim = EnsureGetMember<SpriteAnimationDef>("LoadingTextAnim");
  ResourceBgPos = EnsureGetMember<glm::vec2>("ResourceBgPos");
  SaveBgPos = EnsureGetMember<glm::vec2>("SaveBgPos");
  IconPos = EnsureGetMember<glm::vec2>("IconPos");
  TextPos = EnsureGetMember<glm::vec2>("TextPos");
  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

  Pop();
}

}  // namespace LoadingDisplay
}  // namespace Profile
}  // namespace Impacto