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
  auto const& _loadingDisplay =
      EnsureGetMemberOfType(Json, "/", "LoadingDisplay", kObjectType);

  ResourceLoadBgAnim = EnsureGetMemberAnimation(
      _loadingDisplay, "/LoadingDisplay", "ResourceLoadBgAnim");
  SaveLoadBgAnim = EnsureGetMemberAnimation(_loadingDisplay, "/LoadingDisplay",
                                            "SaveLoadBgAnim");
  LoadingIconAnim = EnsureGetMemberAnimation(_loadingDisplay, "/LoadingDisplay",
                                             "LoadingIconAnim");
  LoadingTextAnim = EnsureGetMemberAnimation(_loadingDisplay, "/LoadingDisplay",
                                             "LoadingTextAnim");
  ResourceBgPos =
      EnsureGetMemberVec2(_loadingDisplay, "/LoadingDisplay", "ResourceBgPos");
  SaveBgPos =
      EnsureGetMemberVec2(_loadingDisplay, "/LoadingDisplay", "SaveBgPos");
  IconPos = EnsureGetMemberVec2(_loadingDisplay, "/LoadingDisplay", "IconPos");
  TextPos = EnsureGetMemberVec2(_loadingDisplay, "/LoadingDisplay", "TextPos");
  FadeInDuration = EnsureGetMemberFloat(_loadingDisplay, "/LoadingDisplay",
                                        "FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat(_loadingDisplay, "/LoadingDisplay",
                                         "FadeOutDuration");
}

}  // namespace LoadingDisplay
}  // namespace Profile
}  // namespace Impacto