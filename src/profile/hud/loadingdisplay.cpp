#include "loadingdisplay.h"
#include "../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace LoadingDisplay {

SpriteAnimationDef ResourceLoadBgAnim;
SpriteAnimationDef SaveLoadBgAnim;
SpriteAnimationDef LoadingIconAnim;
SpriteAnimationDef LoadingTextAnim;

glm::vec2 ResourceBgPos;
glm::vec2 SaveBgPos;
glm::vec2 IconPos;
glm::vec2 TextPos;

float FadeInDuration;
float FadeOutDuration;

void Configure() {
  EnsurePushMemberOfType("LoadingDisplay", kObjectType);

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