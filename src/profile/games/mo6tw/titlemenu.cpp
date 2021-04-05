#include "titlemenu.h"
#include "../../../log.h"
#include "../../../window.h"
#include "../../profile_internal.h"

#include "../../ui/titlemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/mo6tw/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TitleMenu {

Sprite BackgroundSprite;
Sprite CopyrightSprite;
Sprite LogoSprite;
Sprite MenuBackgroundSprite;
Sprite MenuItemLockedSprite;
Sprite MenuItemLockedSpriteH;

float MenuEntriesX;
float MenuEntriesFirstY;
float MenuEntriesYPadding;
float MenuEntriesTargetWidth;
float LogoX;
float LogoY;
float CopyrightX;
float CopyrightY;
float PrimaryFadeAnimDuration;
float SecondaryMenuAnimTargetX;
float SecondaryMenuAnimTargetY;
float SecondaryMenuAnimTarget2X;
float SecondaryMenuAnimTarget2Y;
float SecondaryMenuAnimDuration;
float SecondaryMenuAnimUnderX;
float SecondaryMenuAnimUnderYMultiplier;

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  CopyrightSprite = EnsureGetMemberSprite("CopyrightSprite");
  LogoSprite = EnsureGetMemberSprite("LogoSprite");
  LogoX = EnsureGetMemberFloat("LogoX");
  LogoY = EnsureGetMemberFloat("LogoY");
  CopyrightX = EnsureGetMemberFloat("CopyrightX");
  CopyrightY = EnsureGetMemberFloat("CopyrightY");
  MenuBackgroundSprite = EnsureGetMemberSprite("MenuBackgroundSprite");
  MenuItemLockedSprite = EnsureGetMemberSprite("MenuItemLockedSprite");
  MenuItemLockedSpriteH = EnsureGetMemberSprite("MenuItemLockedSpriteH");
  MenuEntriesTargetWidth = EnsureGetMemberFloat("MenuEntriesTargetWidth");
  MenuEntriesX = EnsureGetMemberFloat("MenuEntriesX");
  MenuEntriesFirstY = EnsureGetMemberFloat("MenuEntriesFirstY");
  MenuEntriesYPadding = EnsureGetMemberFloat("MenuEntriesYPadding");
  PrimaryFadeAnimDuration = EnsureGetMemberFloat("PrimaryFadeAnimDuration");
  SecondaryMenuAnimTargetX = EnsureGetMemberFloat("SecondaryMenuAnimTargetX");
  SecondaryMenuAnimTargetY = EnsureGetMemberFloat("SecondaryMenuAnimTargetY");
  SecondaryMenuAnimTarget2X = EnsureGetMemberFloat("SecondaryMenuAnimTarget2X");
  SecondaryMenuAnimTarget2Y = EnsureGetMemberFloat("SecondaryMenuAnimTarget2Y");
  SecondaryMenuAnimDuration = EnsureGetMemberFloat("SecondaryMenuAnimDuration");
  SecondaryMenuAnimUnderX = EnsureGetMemberFloat("SecondaryMenuAnimUnderX");
  SecondaryMenuAnimUnderYMultiplier =
      EnsureGetMemberFloat("SecondaryMenuAnimUnderYMultiplier");

  UI::MO6TW::TitleMenu* menu = new UI::MO6TW::TitleMenu();
  menu->PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  menu->PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  menu->PressToStartAnimation.LoopMode = ALM_ReverseDirection;
  menu->PrimaryFadeAnimation.DurationIn = PrimaryFadeAnimDuration;
  menu->PrimaryFadeAnimation.DurationOut = PrimaryFadeAnimDuration;
  menu->SecondaryFadeAnimation.DurationIn = SecondaryMenuAnimDuration;
  menu->SecondaryFadeAnimation.DurationOut = SecondaryMenuAnimDuration;
  UI::TitleMenuPtr = menu;
}

}  // namespace TitleMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto