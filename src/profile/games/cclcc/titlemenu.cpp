#include "titlemenu.h"
#include "../../../log.h"
#include "../../../window.h"
#include "../../profile_internal.h"

#include "../../ui/titlemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/cclcc/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TitleMenu {

Sprite BackgroundSprite;
Sprite MainBackgroundSprite;
Sprite CopyrightTextSprite;
Sprite OverlaySprite;
Sprite SmokeSprite;
Sprite MenuSprite;
Sprite ItemHighlightSprite;
Sprite ItemHighlightPointerSprite;
Sprite LoadSprite;
Sprite LoadHighlightSprite;
Sprite QuickLoadSprite;
Sprite QuickLoadHighlightSprite;
Sprite TipsSprite;
Sprite TipsHighlightSprite;
Sprite LibrarySprite;
Sprite LibraryHighlightSprite;
Sprite EndingListSprite;
Sprite EndingListHighlightSprite;

float ItemFadeInDuration;
float ItemFadeOutDuration;
float SecondaryItemFadeInDuration;
float SecondaryItemFadeOutDuration;
float PrimaryFadeInDuration;
float PrimaryFadeOutDuration;
float SecondaryFadeInDuration;
float SecondaryFadeOutDuration;
float CopyrightTextX;
float CopyrightTextY;
float SmokeOpacityNormal;
float SmokeX;
float SmokeY;
float SmokeBoundsX;
float SmokeBoundsY;
float SmokeBoundsWidth;
float SmokeBoundsHeight;
float SmokeAnimationBoundsXOffset;
float SmokeAnimationBoundsXMax;
float SmokeAnimationDurationIn;
float SmokeAnimationDurationOut;
float MenuX;
float MenuY;
float ItemHighlightOffsetX;
float ItemHighlightOffsetY;
float ItemPadding;
float ItemYBase;
float SecondaryFirstItemHighlightOffsetX;
float SecondarySecondItemHighlightOffsetX;
float SecondaryThirdItemHighlightOffsetX;
float ItemHighlightPointerY;

static void GetMemberSpriteArray(Sprite* arr, uint32_t count,
                                 char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d sprites for %s\n", count,
           name);
    Window::Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementSprite(i);
  }

  Pop();
}

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  MainBackgroundSprite = EnsureGetMemberSprite("MainBackgroundSprite");
  CopyrightTextSprite = EnsureGetMemberSprite("CopyrightTextSprite");
  OverlaySprite = EnsureGetMemberSprite("OverlaySprite");
  SmokeSprite = EnsureGetMemberSprite("SmokeSprite");
  MenuSprite = EnsureGetMemberSprite("MenuSprite");
  ItemHighlightSprite = EnsureGetMemberSprite("ItemHighlightSprite");
  LoadSprite = EnsureGetMemberSprite("LoadSprite");
  LoadHighlightSprite = EnsureGetMemberSprite("LoadHighlightSprite");
  QuickLoadSprite = EnsureGetMemberSprite("QuickLoadSprite");
  QuickLoadHighlightSprite = EnsureGetMemberSprite("QuickLoadHighlightSprite");
  TipsSprite = EnsureGetMemberSprite("TipsSprite");
  TipsHighlightSprite = EnsureGetMemberSprite("TipsHighlightSprite");
  LibrarySprite = EnsureGetMemberSprite("LibrarySprite");
  LibraryHighlightSprite = EnsureGetMemberSprite("LibraryHighlightSprite");
  EndingListSprite = EnsureGetMemberSprite("EndingListSprite");
  EndingListHighlightSprite =
      EnsureGetMemberSprite("EndingListHighlightSprite");

  ItemFadeInDuration = EnsureGetMemberFloat("ItemFadeInDuration");
  ItemFadeOutDuration = EnsureGetMemberFloat("ItemFadeOutDuration");
  SecondaryItemFadeInDuration =
      EnsureGetMemberFloat("SecondaryItemFadeInDuration");
  SecondaryItemFadeOutDuration =
      EnsureGetMemberFloat("SecondaryItemFadeOutDuration");
  PrimaryFadeInDuration = EnsureGetMemberFloat("PrimaryFadeInDuration");
  PrimaryFadeOutDuration = EnsureGetMemberFloat("PrimaryFadeOutDuration");
  SecondaryFadeInDuration = EnsureGetMemberFloat("SecondaryFadeInDuration");
  SecondaryFadeOutDuration = EnsureGetMemberFloat("SecondaryFadeOutDuration");
  CopyrightTextX = EnsureGetMemberFloat("CopyrightTextX");
  CopyrightTextY = EnsureGetMemberFloat("CopyrightTextY");
  SmokeOpacityNormal = EnsureGetMemberFloat("SmokeOpacityNormal");
  SmokeX = EnsureGetMemberFloat("SmokeX");
  SmokeY = EnsureGetMemberFloat("SmokeY");
  SmokeBoundsX = EnsureGetMemberFloat("SmokeBoundsX");
  SmokeBoundsY = EnsureGetMemberFloat("SmokeBoundsY");
  SmokeBoundsWidth = EnsureGetMemberFloat("SmokeBoundsWidth");
  SmokeBoundsHeight = EnsureGetMemberFloat("SmokeBoundsHeight");
  SmokeAnimationBoundsXOffset =
      EnsureGetMemberFloat("SmokeAnimationBoundsXOffset");
  SmokeAnimationBoundsXMax = EnsureGetMemberFloat("SmokeAnimationBoundsXMax");
  SmokeAnimationDurationIn = EnsureGetMemberFloat("SmokeAnimationDurationIn");
  SmokeAnimationDurationOut = EnsureGetMemberFloat("SmokeAnimationDurationOut");
  MenuX = EnsureGetMemberFloat("MenuX");
  MenuY = EnsureGetMemberFloat("MenuY");
  ItemHighlightOffsetX = EnsureGetMemberFloat("ItemHighlightOffsetX");
  ItemHighlightOffsetY = EnsureGetMemberFloat("ItemHighlightOffsetY");
  ItemPadding = EnsureGetMemberFloat("ItemPadding");
  ItemYBase = EnsureGetMemberFloat("ItemYBase");
  SecondaryFirstItemHighlightOffsetX =
      EnsureGetMemberFloat("SecondaryFirstItemHighlightOffsetX");
  SecondarySecondItemHighlightOffsetX =
      EnsureGetMemberFloat("SecondarySecondItemHighlightOffsetX");
  SecondaryThirdItemHighlightOffsetX =
      EnsureGetMemberFloat("SecondaryThirdItemHighlightOffsetX");
  ItemHighlightPointerSprite =
      EnsureGetMemberSprite("ItemHighlightPointerSprite");
  ItemHighlightPointerY = EnsureGetMemberFloat("ItemHighlightPointerY");

  UI::CCLCC::TitleMenu* menu = new UI::CCLCC::TitleMenu();
  menu->PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  menu->PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  menu->PressToStartAnimation.LoopMode = ALM_ReverseDirection;

  menu->ItemsFadeInAnimation.DurationIn = ItemFadeInDuration;
  menu->ItemsFadeInAnimation.DurationOut = ItemFadeOutDuration;

  menu->SecondaryItemsFadeInAnimation.DurationIn = SecondaryItemFadeInDuration;
  menu->SecondaryItemsFadeInAnimation.DurationOut =
      SecondaryItemFadeOutDuration;

  menu->PrimaryFadeAnimation.DurationIn = PrimaryFadeInDuration;
  menu->PrimaryFadeAnimation.DurationOut = PrimaryFadeOutDuration;
  menu->SecondaryFadeAnimation.DurationIn = SecondaryFadeInDuration;
  menu->SecondaryFadeAnimation.DurationOut = SecondaryFadeOutDuration;

  menu->SmokeAnimation.LoopMode = ALM_Loop;
  menu->SmokeAnimation.DurationIn = SmokeAnimationDurationIn;
  menu->SmokeAnimation.DurationOut = SmokeAnimationDurationOut;

  UI::TitleMenuPtr = menu;

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto