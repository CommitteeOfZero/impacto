#include "titlemenu.h"
#include "../../profile_internal.h"

#include "../../ui/titlemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/dash/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace Dash {
namespace TitleMenu {

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  PressToStartAnimation.LoopMode = ALM_ReverseDirection;

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::TitleMenuPtr = new UI::Dash::TitleMenu();
  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace Dash
}  // namespace Profile
}  // namespace Impacto