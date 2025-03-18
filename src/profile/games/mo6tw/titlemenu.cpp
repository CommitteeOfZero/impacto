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
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  CopyrightSprite = EnsureGetMember<Sprite>("CopyrightSprite");
  LogoSprite = EnsureGetMember<Sprite>("LogoSprite");
  LogoX = EnsureGetMember<float>("LogoX");
  LogoY = EnsureGetMember<float>("LogoY");
  CopyrightX = EnsureGetMember<float>("CopyrightX");
  CopyrightY = EnsureGetMember<float>("CopyrightY");
  MenuBackgroundSprite = EnsureGetMember<Sprite>("MenuBackgroundSprite");
  MenuItemLockedSprite = EnsureGetMember<Sprite>("MenuItemLockedSprite");
  MenuItemLockedSpriteH = EnsureGetMember<Sprite>("MenuItemLockedSpriteH");
  MenuEntriesTargetWidth = EnsureGetMember<float>("MenuEntriesTargetWidth");
  MenuEntriesX = EnsureGetMember<float>("MenuEntriesX");
  MenuEntriesFirstY = EnsureGetMember<float>("MenuEntriesFirstY");
  MenuEntriesYPadding = EnsureGetMember<float>("MenuEntriesYPadding");
  PrimaryFadeAnimDuration = EnsureGetMember<float>("PrimaryFadeAnimDuration");
  SecondaryMenuAnimTarget =
      EnsureGetMember<glm::vec2>("SecondaryMenuAnimTarget");
  SecondaryMenuPadding = EnsureGetMember<float>("SecondaryMenuPadding");
  SecondaryMenuAnimDuration =
      EnsureGetMember<float>("SecondaryMenuAnimDuration");
  SecondaryMenuAnimUnderX = EnsureGetMember<float>("SecondaryMenuAnimUnderX");
  ExtraStoryItemCount = EnsureGetMember<float>("ExtraStoryItemCount");
  ContinueItemCount = EnsureGetMember<float>("ContinueItemCount");
  MemoriesItemCount = EnsureGetMember<float>("MemoriesItemCount");
  SystemItemCount = EnsureGetMember<float>("SystemItemCount");

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
      EnsureGetMember<int>("DrawType"));

  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto