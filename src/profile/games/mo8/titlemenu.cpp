#include "titlemenu.h"
#include "../../profile_internal.h"

#include "../../ui/titlemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/mo8/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace MO8 {
namespace TitleMenu {

Sprite BackgroundSprite;
Sprite LogoSprite;

float LogoPositionX;
float LogoPositionY;

int NewGameSpriteIndex;
int ContinueSpriteIndex;
int OptionSpriteIndex;

float MenuEntriesX;
float MenuEntriesFirstY;
float MenuEntriesYPadding;

bool PressToStartAnimated;

Animation PressToStartAnimation;
float PrimaryFadeAnimDuration;

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  LogoSprite = EnsureGetMemberSprite("LogoSprite");
  LogoPositionX = EnsureGetMemberFloat("LogoX");
  LogoPositionY = EnsureGetMemberFloat("LogoY");
  NewGameSpriteIndex = EnsureGetMemberInt("NewGameSpriteIndex");
  ContinueSpriteIndex = EnsureGetMemberInt("ContinueSpriteIndex");
  OptionSpriteIndex = EnsureGetMemberInt("OptionSpriteIndex");
  MenuEntriesX = EnsureGetMemberFloat("MenuEntriesX");
  MenuEntriesFirstY = EnsureGetMemberFloat("MenuEntriesFirstY");
  MenuEntriesYPadding = EnsureGetMemberFloat("MenuEntriesYPadding");
  PressToStartAnimated = EnsureGetMemberBool("PressToStartAnimated");
  PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  PressToStartAnimation.LoopMode = ALM_ReverseDirection;
  PrimaryFadeAnimDuration = EnsureGetMemberFloat("PrimaryFadeAnimDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::MO8::TitleMenu* menu = new UI::MO8::TitleMenu();
  menu->PrimaryFadeAnimation.DurationIn = PrimaryFadeAnimDuration;
  menu->PrimaryFadeAnimation.DurationOut = PrimaryFadeAnimDuration;
  UI::TitleMenuPtr = menu;
  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto