#include "titlemenu.h"
#include "../../../log.h"
#include "../../profile_internal.h"

#include "../../ui/titlemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/mo6tw/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TitleMenu {

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
  SecondaryMenuAnimTarget = EnsureGetMemberVec2("SecondaryMenuAnimTarget");
  SecondaryMenuPadding = EnsureGetMemberFloat("SecondaryMenuPadding");
  SecondaryMenuAnimDuration = EnsureGetMemberFloat("SecondaryMenuAnimDuration");
  SecondaryMenuAnimUnderX = EnsureGetMemberFloat("SecondaryMenuAnimUnderX");
  ExtraStoryItemCount = EnsureGetMemberFloat("ExtraStoryItemCount");
  ContinueItemCount = EnsureGetMemberFloat("ContinueItemCount");
  MemoriesItemCount = EnsureGetMemberFloat("MemoriesItemCount");
  SystemItemCount = EnsureGetMemberFloat("SystemItemCount");

  UI::MO6TW::TitleMenu* menu = new UI::MO6TW::TitleMenu();
  menu->PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  menu->PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  menu->PressToStartAnimation.LoopMode = AnimationLoopMode::ReverseDirection;
  menu->PrimaryFadeAnimation.DurationIn = PrimaryFadeAnimDuration;
  menu->PrimaryFadeAnimation.DurationOut = PrimaryFadeAnimDuration;
  menu->SecondaryFadeAnimation.DurationIn = SecondaryMenuAnimDuration;
  menu->SecondaryFadeAnimation.DurationOut = SecondaryMenuAnimDuration;
  UI::TitleMenuPtr = menu;

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto