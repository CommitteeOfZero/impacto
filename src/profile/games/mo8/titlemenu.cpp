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

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  LogoSprite = EnsureGetMember<Sprite>("LogoSprite");
  LogoPositionX = EnsureGetMember<float>("LogoX");
  LogoPositionY = EnsureGetMember<float>("LogoY");
  NewGameSpriteIndex = EnsureGetMember<int>("NewGameSpriteIndex");
  ContinueSpriteIndex = EnsureGetMember<int>("ContinueSpriteIndex");
  OptionSpriteIndex = EnsureGetMember<int>("OptionSpriteIndex");
  GallerySpriteIndex = EnsureGetMember<int>("GallerySpriteIndex");
  AlbumSpriteIndex = EnsureGetMember<int>("AlbumSpriteIndex");
  MusicSpriteIndex = EnsureGetMember<int>("MusicSpriteIndex");
  ClearListSpriteIndex = EnsureGetMember<int>("ClearListSpriteIndex");
  WarningSpriteIndex = EnsureGetMember<int>("WarningSpriteIndex");
  AdditionalSpriteIndex = EnsureGetMember<int>("AdditionalSpriteIndex");
  DLCSpriteIndex = EnsureGetMember<int>("DLCSpriteIndex");
  LoadSpriteIndex = EnsureGetMember<int>("LoadSpriteIndex");
  QuickLoadSpriteIndex = EnsureGetMember<int>("QuickLoadSpriteIndex");
  MenuEntriesX = EnsureGetMember<float>("MenuEntriesX");
  MenuEntriesFirstY = EnsureGetMember<float>("MenuEntriesFirstY");
  MenuEntriesGalleryFirstY = EnsureGetMember<float>("MenuEntriesGalleryFirstY");
  MenuEntriesYPadding = EnsureGetMember<float>("MenuEntriesYPadding");
  HasAdditional = EnsureGetMember<bool>("HasAdditional");
  PressToStartAnimated = EnsureGetMember<bool>("PressToStartAnimated");
  PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  PressToStartAnimation.LoopMode = AnimationLoopMode::ReverseDirection;
  PrimaryFadeAnimDuration = EnsureGetMember<float>("PrimaryFadeAnimDuration");
  ItemFadeAnimDuration = EnsureGetMember<float>("ItemFadeAnimDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::MO8::TitleMenu* menu = new UI::MO8::TitleMenu();
  menu->PrimaryFadeAnimation.DurationIn = PrimaryFadeAnimDuration;
  menu->PrimaryFadeAnimation.DurationOut = PrimaryFadeAnimDuration;
  menu->MainItemsHideAnimation.DurationIn = ItemFadeAnimDuration;
  menu->MainItemsHideAnimation.DurationOut = ItemFadeAnimDuration;
  menu->ContinueItemsShowAnimation.DurationIn = ItemFadeAnimDuration;
  menu->ContinueItemsShowAnimation.DurationOut = ItemFadeAnimDuration;
  menu->GalleryItemsShowAnimation.DurationIn = ItemFadeAnimDuration;
  menu->GalleryItemsShowAnimation.DurationOut = ItemFadeAnimDuration;
  UI::TitleMenuPtr = menu;
  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto