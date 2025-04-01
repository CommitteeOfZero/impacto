#include "tipsmenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../text.h"

#include "../../ui/tipsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/tipsmenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TipsMenu {

void Configure() {
  MenuTransitionDuration = EnsureGetMember<float>("TransitionDuration");
  BackgroundColor = EnsureGetMember<uint32_t>("BackgroundColor");
  CircleSprite = EnsureGetMember<Sprite>("CircleSprite");
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
  MenuTitleTextRightPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextRightPos");
  MenuTitleTextLeftPosition =
      EnsureGetMember<glm::vec2>("MenuTitleTextLeftPos");
  MenuTitleTextAngle = EnsureGetMember<float>("MenuTitleTextAngle");
  TitleFadeInDuration = EnsureGetMember<float>("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMember<float>("TitleFadeOutDuration");
  MenuTitleText = EnsureGetMember<Sprite>("MenuTitleText");
  TreePosition = EnsureGetMember<glm::vec2>("TreePosition");
  TipsTree = EnsureGetMember<Sprite>("TipsTree");
  GradientPosition = EnsureGetMember<glm::vec2>("GradientPosition");
  TipsGradient = EnsureGetMember<Sprite>("TipsGradient");
  EndOfGradientColor = EnsureGetMember<uint32_t>("EndOfGradientColor");
  CurrentTipBackgroundPosition =
      EnsureGetMember<glm::vec2>("CurrentTipBackgroundPosition");
  CurrentTipBackgroundSprite =
      EnsureGetMember<Sprite>("CurrentTipBackgroundSprite");
  ButtonPromptPosition = EnsureGetMember<glm::vec2>("ButtonPromptPosition");
  ButtonPromptSprite = EnsureGetMember<Sprite>("ButtonPromptSprite");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::TipsMenuPtr = new UI::CHLCC::TipsMenu();
  UI::Menus[drawType].push_back(UI::TipsMenuPtr);
}

}  // namespace TipsMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
