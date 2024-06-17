#include "loadingdisplay.h"
#include "../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace LoadingDisplay {

void Configure() {
  EnsurePushMemberOfType("LoadingDisplay", LUA_TTABLE);

  ResourceLoadBgAnim = EnsureGetMemberAnimation("ResourceLoadBgAnim");
  SaveLoadBgAnim = EnsureGetMemberAnimation("SaveLoadBgAnim");
  LoadingIconAnim = EnsureGetMemberAnimation("LoadingIconAnim");
  LoadingTextAnim = EnsureGetMemberAnimation("LoadingTextAnim");
  ResourceBgPos = EnsureGetMemberVec2("ResourceBgPos");
  SaveBgPos = EnsureGetMemberVec2("SaveBgPos");
  IconPos = EnsureGetMemberVec2("IconPos");
  TextPos = EnsureGetMemberVec2("TextPos");
  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

  Pop();
}

}  // namespace LoadingDisplay
}  // namespace Profile
}  // namespace Impacto