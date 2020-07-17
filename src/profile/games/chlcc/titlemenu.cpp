#include "titlemenu.h"
#include "../../../log.h"
#include "../../../window.h"
#include "../../profile_internal.h"

#include "../../ui/titlemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TitleMenu {

Sprite BackgroundSprite;
Sprite DelusionADVUnderSprite;
Sprite DelusionADVSprite;
Sprite SeiraUnderSprite;
Sprite SeiraSprite;
Sprite CHLogoSprite;
Sprite LCCLogoUnderSprite;
Sprite ChuLeftLogoSprite;
Sprite ChuRightLogoSprite;
Sprite LoveLogoSprite;
Sprite StarLogoSprite;
Sprite ExclMarkLogoSprite;
Sprite CopyrightTextSprite;
Sprite SpinningCircleSprite;
Sprite ItemHighlightSprite;
Sprite ItemLoadLineSprite;
Sprite ItemLoadQuickLineSprite;
Sprite ItemLoadQuickSprite;
Sprite ItemLoadSprite;
Sprite ItemLoadQuickHighlightedSprite;
Sprite ItemLoadHighlightedSprite;
Sprite SecondaryItemHighlightSprite;

float DelusionADVUnderX;
float DelusionADVUnderY;
float DelusionADVX;
float DelusionADVY;
float SeiraUnderX;
float SeiraUnderY;
float SeiraX;
float SeiraY;
float CHLogoX;
float CHLogoY;
float LCCLogoUnderX;
float LCCLogoUnderY;
float ChuLeftLogoX;
float ChuLeftLogoY;
float ChuRightLogoX;
float ChuRightLogoY;
float LoveLogoX;
float LoveLogoY;
float StarLogoX;
float StarLogoY;
float ExclMarkLogoX;
float ExclMarkLogoY;
float CopyrightTextX;
float CopyrightTextY;
float SpinningCircleX;
float SpinningCircleY;
float SpinningCircleAnimationDuration;
float ItemHighlightOffsetX;
float ItemHighlightOffsetY;
float ItemPadding;
float ItemYBase;
float ItemFadeInDuration;
float ItemFadeOutDuration;
float ItemLoadX;
float ItemLoadY;
float ItemLoadQuickY;
float SecondaryItemHighlightX;
float SecondaryMenuPaddingY;
float SecondaryMenuLoadOffsetY;
float SecondaryMenuLoadLineX;
float SecondaryMenuLoadLineY;
float SecondaryMenuLoadQuickLineY;

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
  DelusionADVUnderSprite = EnsureGetMemberSprite("DelusionADVUnderSprite");
  DelusionADVUnderX = EnsureGetMemberFloat("DelusionADVUnderX");
  DelusionADVUnderY = EnsureGetMemberFloat("DelusionADVUnderY");
  DelusionADVSprite = EnsureGetMemberSprite("DelusionADVSprite");
  DelusionADVX = EnsureGetMemberFloat("DelusionADVX");
  DelusionADVY = EnsureGetMemberFloat("DelusionADVY");
  SeiraUnderSprite = EnsureGetMemberSprite("SeiraUnderSprite");
  SeiraUnderX = EnsureGetMemberFloat("SeiraUnderX");
  SeiraUnderY = EnsureGetMemberFloat("SeiraUnderY");
  SeiraSprite = EnsureGetMemberSprite("SeiraSprite");
  SeiraX = EnsureGetMemberFloat("SeiraX");
  SeiraY = EnsureGetMemberFloat("SeiraY");
  CHLogoSprite = EnsureGetMemberSprite("CHLogoSprite");
  CHLogoX = EnsureGetMemberFloat("CHLogoX");
  CHLogoY = EnsureGetMemberFloat("CHLogoY");
  LCCLogoUnderSprite = EnsureGetMemberSprite("LCCLogoUnderSprite");
  LCCLogoUnderX = EnsureGetMemberFloat("LCCLogoUnderX");
  LCCLogoUnderY = EnsureGetMemberFloat("LCCLogoUnderY");
  ChuLeftLogoSprite = EnsureGetMemberSprite("ChuLeftLogoSprite");
  ChuLeftLogoX = EnsureGetMemberFloat("ChuLeftLogoX");
  ChuLeftLogoY = EnsureGetMemberFloat("ChuLeftLogoY");
  ChuRightLogoSprite = EnsureGetMemberSprite("ChuRightLogoSprite");
  ChuRightLogoX = EnsureGetMemberFloat("ChuRightLogoX");
  ChuRightLogoY = EnsureGetMemberFloat("ChuRightLogoY");
  LoveLogoSprite = EnsureGetMemberSprite("LoveLogoSprite");
  LoveLogoX = EnsureGetMemberFloat("LoveLogoX");
  LoveLogoY = EnsureGetMemberFloat("LoveLogoY");
  StarLogoSprite = EnsureGetMemberSprite("StarLogoSprite");
  StarLogoX = EnsureGetMemberFloat("StarLogoX");
  StarLogoY = EnsureGetMemberFloat("StarLogoY");
  ExclMarkLogoSprite = EnsureGetMemberSprite("ExclMarkLogoSprite");
  ExclMarkLogoX = EnsureGetMemberFloat("ExclMarkLogoX");
  ExclMarkLogoY = EnsureGetMemberFloat("ExclMarkLogoY");
  CopyrightTextSprite = EnsureGetMemberSprite("CopyrightTextSprite");
  CopyrightTextX = EnsureGetMemberFloat("CopyrightTextX");
  CopyrightTextY = EnsureGetMemberFloat("CopyrightTextY");
  SpinningCircleSprite = EnsureGetMemberSprite("SpinningCircleSprite");
  SpinningCircleX = EnsureGetMemberFloat("SpinningCircleX");
  SpinningCircleY = EnsureGetMemberFloat("SpinningCircleY");
  SpinningCircleAnimationDuration =
      EnsureGetMemberFloat("SpinningCircleAnimationDuration");
  ItemHighlightSprite = EnsureGetMemberSprite("ItemHighlightSprite");
  ItemHighlightOffsetX = EnsureGetMemberFloat("ItemHighlightOffsetX");
  ItemHighlightOffsetY = EnsureGetMemberFloat("ItemHighlightOffsetY");
  ItemPadding = EnsureGetMemberFloat("ItemPadding");
  ItemYBase = EnsureGetMemberFloat("ItemYBase");
  ItemFadeInDuration = EnsureGetMemberFloat("ItemFadeInDuration");
  ItemFadeOutDuration = EnsureGetMemberFloat("ItemFadeOutDuration");
  ItemLoadLineSprite = EnsureGetMemberSprite("ItemLoadLineSprite");
  ItemLoadQuickLineSprite = EnsureGetMemberSprite("ItemLoadQuickLineSprite");
  ItemLoadQuickSprite = EnsureGetMemberSprite("ItemLoadQuickSprite");
  ItemLoadX = EnsureGetMemberFloat("ItemLoadX");
  ItemLoadY = EnsureGetMemberFloat("ItemLoadY");
  ItemLoadQuickY = EnsureGetMemberFloat("ItemLoadQuickY");
  ItemLoadSprite = EnsureGetMemberSprite("ItemLoadSprite");
  ItemLoadQuickHighlightedSprite =
      EnsureGetMemberSprite("ItemLoadQuickHighlightedSprite");
  ItemLoadHighlightedSprite =
      EnsureGetMemberSprite("ItemLoadHighlightedSprite");
  SecondaryItemHighlightSprite =
      EnsureGetMemberSprite("SecondaryItemHighlightSprite");
  SecondaryItemHighlightX = EnsureGetMemberFloat("SecondaryItemHighlightX");
  SecondaryMenuPaddingY = EnsureGetMemberFloat("SecondaryMenuPaddingY");
  SecondaryMenuLoadOffsetY = EnsureGetMemberFloat("SecondaryMenuLoadOffsetY");
  SecondaryMenuLoadLineX = EnsureGetMemberFloat("SecondaryMenuLoadLineX");
  SecondaryMenuLoadLineY = EnsureGetMemberFloat("SecondaryMenuLoadLineY");
  SecondaryMenuLoadQuickLineY =
      EnsureGetMemberFloat("SecondaryMenuLoadQuickLineY");

  UI::CHLCC::TitleMenu* menu = new UI::CHLCC::TitleMenu();
  menu->PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  menu->PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  menu->PressToStartAnimation.LoopMode = ALM_ReverseDirection;

  menu->ItemsFadeInAnimation.DurationIn = ItemFadeInDuration;
  menu->ItemsFadeInAnimation.DurationOut = ItemFadeOutDuration;

  menu->SecondaryItemsFadeInAnimation.DurationIn = 0.2f;
  menu->SecondaryItemsFadeInAnimation.DurationOut = 0.2f;

  menu->SpinningCircleAnimation.LoopMode = ALM_Loop;
  menu->SpinningCircleAnimation.DurationIn = SpinningCircleAnimationDuration;
  menu->SpinningCircleAnimation.DurationOut = SpinningCircleAnimationDuration;

  UI::TitleMenuPtr = menu;
}

}  // namespace TitleMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto