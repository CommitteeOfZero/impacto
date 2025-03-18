#include "titlemenu.h"
#include "../../profile_internal.h"
#include "tilebackground.h"

#include "../../ui/titlemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/rne/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace RNE {
namespace TitleMenu {

void Configure() {
  EnsurePushMemberOfType("Background", LUA_TTABLE);
  BackgroundAnimation = RNE::ParseTileBackground();
  Pop();

  LineSprite = EnsureGetMember<Sprite>("LineSprite");
  CopyrightSprite = EnsureGetMember<Sprite>("CopyrightSprite");
  EliteSprite = EnsureGetMember<Sprite>("EliteSprite");
  LogoSprite = EnsureGetMember<Sprite>("LogoSprite");

  LineWidth = EnsureGetMember<float>("LineWidth");
  CopyrightWidth = EnsureGetMember<float>("CopyrightWidth");
  EliteHeight = EnsureGetMember<float>("EliteHeight");
  LogoWidth = EnsureGetMember<float>("LogoWidth");
  LineX = EnsureGetMember<float>("LineX");
  LineY = EnsureGetMember<float>("LineY");
  CopyrightX = EnsureGetMember<float>("CopyrightX");
  CopyrightY = EnsureGetMember<float>("CopyrightY");
  EliteX = EnsureGetMember<float>("EliteX");
  EliteY = EnsureGetMember<float>("EliteY");
  LogoX = EnsureGetMember<float>("LogoX");
  LogoY = EnsureGetMember<float>("LogoY");

  PreTitleAnimDurationIn = EnsureGetMember<float>("PreTitleAnimDurationIn");
  PreTitleAnimDurationOut = EnsureGetMember<float>("PreTitleAnimDurationOut");

  PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  PressToStartAnimation.LoopMode = AnimationLoopMode::ReverseDirection;
  PreTitleItemsAnimation.DurationIn = PreTitleAnimDurationIn;
  PreTitleItemsAnimation.DurationOut = PreTitleAnimDurationOut;

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::TitleMenuPtr = new UI::RNE::TitleMenu();
  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto