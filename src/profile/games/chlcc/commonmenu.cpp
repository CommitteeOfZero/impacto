#include "commonmenu.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace CommonMenu {

void Configure() {
  TitleFadeInDuration = EnsureGetMember<float>("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMember<float>("TitleFadeOutDuration");

  CircleStartPosition = EnsureGetMember<glm::vec2>("CircleStartPosition");
  CircleOffset = EnsureGetMember<float>("CircleOffset");

  ErinPosition = EnsureGetMember<glm::vec2>("ErinPosition");
  ErinSprite = EnsureGetMember<Sprite>("ErinSprite");

  BackgroundFilter = EnsureGetMember<Sprite>("BackgroundFilter");

  InitialRedBarPosition = EnsureGetMember<glm::vec2>("InitialRedBarPosition");
  RightRedBarPosition = EnsureGetMember<glm::vec2>("RightRedBarPosition");
  InitialRedBarSprite = EnsureGetMember<Sprite>("RedBarSprite");
  RedBarDivision = EnsureGetMember<float>("RedBarDivision");
  RedBarBaseX = EnsureGetMember<float>("RedBarBaseX");
  RedBarLabelPosition = EnsureGetMember<glm::vec2>("RedBarLabelPosition");
  RedBarLabel = EnsureGetMember<Sprite>("RedBarLabel");

  MenuSelectPromptDuration = EnsureGetMember<float>("MenuSelectPromptDuration");
  MenuSelectPromptInterval = EnsureGetMember<float>("MenuSelectPromptInterval");
  MenuTransitionDuration = EnsureGetMember<float>("MenuTransitionDuration");
  ShowPageAnimationStartTime =
      EnsureGetMember<float>("ShowPageAnimationStartTime");
  ShowPageAnimationDuration =
      EnsureGetMember<float>("ShowPageAnimationDuration");
  ButtonPromptAnimationStartTime =
      EnsureGetMember<float>("ButtonPromptAnimationStartTime");
  ButtonPromptAnimationDuration =
      EnsureGetMember<float>("ButtonPromptAnimationDuration");
  ButtonPromptStartPosition =
      EnsureGetMember<glm::vec2>("ButtonPromptStartPosition");
  DiagonalTitlesOffsetStart =
      EnsureGetMember<glm::vec2>("DiagonalTitlesOffsetStart");
  DiagonalTitlesOffsetEnd =
      EnsureGetMember<glm::vec2>("DiagonalTitlesOffsetEnd");
}

}  // namespace CommonMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto