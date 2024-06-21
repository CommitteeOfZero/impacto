#include "trophymenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../text.h"

#include "../../ui/trophymenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/trophymenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TrophyMenu {

void Configure() {
  MenuTransitionDuration = EnsureGetMemberFloat("TransitionDuration");
  BackgroundColor = EnsureGetMemberUint("BackgroundColor");
  CircleSprite = EnsureGetMemberSprite("CircleSprite");
  CircleStartPosition = EnsureGetMemberVec2("CircleStartPosition");
  CircleOffset = EnsureGetMemberFloat("CircleOffset");
  ErinPosition = EnsureGetMemberVec2("ErinPosition");
  ErinSprite = EnsureGetMemberSprite("ErinSprite");
  BackgroundFilter = EnsureGetMemberSprite("BackgroundFilter");
  InitialRedBarPosition = EnsureGetMemberVec2("InitialRedBarPosition");
  RightRedBarPosition = EnsureGetMemberVec2("RightRedBarPosition");
  InitialRedBarSprite = EnsureGetMemberSprite("RedBarSprite");
  RedBarDivision = EnsureGetMemberFloat("RedBarDivision");
  RedBarBaseX = EnsureGetMemberFloat("RedBarBaseX");
  RedBarLabelPosition = EnsureGetMemberVec2("RedBarLabelPosition");
  RedBarLabel = EnsureGetMemberSprite("RedBarLabel");
  MenuTitleTextRightPosition = EnsureGetMemberVec2("MenuTitleTextRightPos");
  MenuTitleTextLeftPosition = EnsureGetMemberVec2("MenuTitleTextLeftPos");
  MenuTitleTextAngle = EnsureGetMemberFloat("MenuTitleTextAngle");
  TitleFadeInDuration = EnsureGetMemberFloat("TitleFadeInDuration");
  TitleFadeOutDuration = EnsureGetMemberFloat("TitleFadeOutDuration");
  MenuTitleText = EnsureGetMemberSprite("MenuTitleText");
  ButtonPromptPosition = EnsureGetMemberVec2("ButtonPromptPosition");
  ButtonPromptSprite = EnsureGetMemberSprite("ButtonPromptSprite");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::TrophyMenuPtr = new UI::CHLCC::TrophyMenu();
  UI::Menus[drawType].push_back(UI::TrophyMenuPtr);
}

}  // namespace OptionsMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
