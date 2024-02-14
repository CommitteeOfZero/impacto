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

Sprite LineSprite;
Sprite CopyrightSprite;
Sprite EliteSprite;
Sprite LogoSprite;

float PreTitleAnimDurationIn;
float PreTitleAnimDurationOut;

float LineWidth;
float CopyrightWidth;
float EliteHeight;
float LogoWidth;
float LineX;
float LineY;
float CopyrightX;
float CopyrightY;
float EliteX;
float EliteY;
float LogoX;
float LogoY;

Animation* BackgroundAnimation = 0;
Animation PreTitleItemsAnimation;
Animation PressToStartAnimation;

void Configure() {
  EnsurePushMemberOfType("Background", LUA_TTABLE);
  BackgroundAnimation = RNE::ParseTileBackground();
  Pop();

  LineSprite = EnsureGetMemberSprite("LineSprite");
  CopyrightSprite = EnsureGetMemberSprite("CopyrightSprite");
  EliteSprite = EnsureGetMemberSprite("EliteSprite");
  LogoSprite = EnsureGetMemberSprite("LogoSprite");

  LineWidth = EnsureGetMemberFloat("LineWidth");
  CopyrightWidth = EnsureGetMemberFloat("CopyrightWidth");
  EliteHeight = EnsureGetMemberFloat("EliteHeight");
  LogoWidth = EnsureGetMemberFloat("LogoWidth");
  LineX = EnsureGetMemberFloat("LineX");
  LineY = EnsureGetMemberFloat("LineY");
  CopyrightX = EnsureGetMemberFloat("CopyrightX");
  CopyrightY = EnsureGetMemberFloat("CopyrightY");
  EliteX = EnsureGetMemberFloat("EliteX");
  EliteY = EnsureGetMemberFloat("EliteY");
  LogoX = EnsureGetMemberFloat("LogoX");
  LogoY = EnsureGetMemberFloat("LogoY");

  PreTitleAnimDurationIn = EnsureGetMemberFloat("PreTitleAnimDurationIn");
  PreTitleAnimDurationOut = EnsureGetMemberFloat("PreTitleAnimDurationOut");

  PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  PressToStartAnimation.LoopMode = ALM_ReverseDirection;
  PreTitleItemsAnimation.DurationIn = PreTitleAnimDurationIn;
  PreTitleItemsAnimation.DurationOut = PreTitleAnimDurationOut;

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::TitleMenuPtr = new UI::RNE::TitleMenu();
  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace RNE
}  // namespace Profile
}  // namespace Impacto