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
int GallerySpriteIndex;
int AlbumSpriteIndex;
int MusicSpriteIndex;
int ClearListSpriteIndex;
int WarningSpriteIndex;
int AdditionalSpriteIndex;
int DLCSpriteIndex;
int LoadSpriteIndex;
int QuickLoadSpriteIndex;

float MenuEntriesX;
float MenuEntriesFirstY;
float MenuEntriesGalleryFirstY;
float MenuEntriesYPadding;

bool PressToStartAnimated;
bool HasAdditional;

Animation PressToStartAnimation;
float PrimaryFadeAnimDuration;
float ItemFadeAnimDuration;

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  LogoSprite = EnsureGetMemberSprite("LogoSprite");
  LogoPositionX = EnsureGetMemberFloat("LogoX");
  LogoPositionY = EnsureGetMemberFloat("LogoY");
  NewGameSpriteIndex = EnsureGetMemberInt("NewGameSpriteIndex");
  ContinueSpriteIndex = EnsureGetMemberInt("ContinueSpriteIndex");
  OptionSpriteIndex = EnsureGetMemberInt("OptionSpriteIndex");
  GallerySpriteIndex = EnsureGetMemberInt("GallerySpriteIndex");
  AlbumSpriteIndex = EnsureGetMemberInt("AlbumSpriteIndex");
  MusicSpriteIndex = EnsureGetMemberInt("MusicSpriteIndex");
  ClearListSpriteIndex = EnsureGetMemberInt("ClearListSpriteIndex");
  WarningSpriteIndex = EnsureGetMemberInt("WarningSpriteIndex");
  AdditionalSpriteIndex = EnsureGetMemberInt("AdditionalSpriteIndex");
  DLCSpriteIndex = EnsureGetMemberInt("DLCSpriteIndex");
  LoadSpriteIndex = EnsureGetMemberInt("LoadSpriteIndex");
  QuickLoadSpriteIndex = EnsureGetMemberInt("QuickLoadSpriteIndex");
  MenuEntriesX = EnsureGetMemberFloat("MenuEntriesX");
  MenuEntriesFirstY = EnsureGetMemberFloat("MenuEntriesFirstY");
  MenuEntriesGalleryFirstY = EnsureGetMemberFloat("MenuEntriesGalleryFirstY");
  MenuEntriesYPadding = EnsureGetMemberFloat("MenuEntriesYPadding");
  HasAdditional = EnsureGetMemberBool("HasAdditional");
  PressToStartAnimated = EnsureGetMemberBool("PressToStartAnimated");
  PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  PressToStartAnimation.LoopMode = ALM_ReverseDirection;
  PrimaryFadeAnimDuration = EnsureGetMemberFloat("PrimaryFadeAnimDuration");
  ItemFadeAnimDuration = EnsureGetMemberFloat("ItemFadeAnimDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

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