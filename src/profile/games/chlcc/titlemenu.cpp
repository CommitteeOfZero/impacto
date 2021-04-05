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
Sprite LineSprites[LineEntriesNumMax];
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
float SecondaryItemFadeInDuration;
float SecondaryItemFadeOutDuration;
float PrimaryFadeInDuration;
float PrimaryFadeOutDuration;
float SecondaryFadeInDuration;
float SecondaryFadeOutDuration;
float SecondaryItemX;
float ItemLoadY;
float ItemLoadQuickY;
float ItemClearListY;
float ItemCGLibraryY;
float ItemSoundLibraryY;
float ItemMovieLibraryY;
float ItemTipsY;
float ItemTrophyY;
float ItemConfigY;
float ItemSystemSaveY;
float SecondaryItemHighlightX;
float SecondaryMenuPaddingY;
float SecondaryMenuLoadOffsetY;
float SecondaryMenuLineX;
float SecondaryMenuLoadLineY;
float SecondaryMenuLoadQuickLineY;
float SecondaryMenuExtraClearY;
float SecondaryMenuExtraCGY;
float SecondaryMenuExtraSoundY;
float SecondaryMenuExtraMovieY;
float SecondaryMenuExtraTipsY;
float SecondaryMenuExtraTrophyY;
float SecondaryMenuSystemConfigY;
float SecondaryMenuSystemSaveY;

int LineNum;

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
  SecondaryItemFadeInDuration =
      EnsureGetMemberFloat("SecondaryItemFadeInDuration");
  SecondaryItemFadeOutDuration =
      EnsureGetMemberFloat("SecondaryItemFadeOutDuration");
  PrimaryFadeInDuration = EnsureGetMemberFloat("PrimaryFadeInDuration");
  PrimaryFadeOutDuration = EnsureGetMemberFloat("PrimaryFadeOutDuration");
  SecondaryFadeInDuration = EnsureGetMemberFloat("SecondaryFadeInDuration");
  SecondaryFadeOutDuration = EnsureGetMemberFloat("SecondaryFadeOutDuration");
  LineNum = EnsureGetMemberInt("LineNum");
  GetMemberSpriteArray(LineSprites, LineNum, "LineEntriesSprites");
  ItemLoadQuickSprite = EnsureGetMemberSprite("ItemLoadQuickSprite");
  SecondaryItemX = EnsureGetMemberFloat("SecondaryItemX");
  ItemLoadY = EnsureGetMemberFloat("ItemLoadY");
  ItemLoadQuickY = EnsureGetMemberFloat("ItemLoadQuickY");
  ItemLoadSprite = EnsureGetMemberSprite("ItemLoadSprite");
  ItemLoadQuickHighlightedSprite =
      EnsureGetMemberSprite("ItemLoadQuickHighlightedSprite");
  ItemLoadHighlightedSprite =
      EnsureGetMemberSprite("ItemLoadHighlightedSprite");
  ItemClearListY = EnsureGetMemberFloat("ItemClearListY");
  ItemCGLibraryY = EnsureGetMemberFloat("ItemCGLibraryY");
  ItemSoundLibraryY = EnsureGetMemberFloat("ItemSoundLibraryY");
  ItemMovieLibraryY = EnsureGetMemberFloat("ItemMovieLibraryY");
  ItemTipsY = EnsureGetMemberFloat("ItemTipsY");
  ItemTrophyY = EnsureGetMemberFloat("ItemTrophyY");
  ItemConfigY = EnsureGetMemberFloat("ItemConfigY");
  ItemSystemSaveY = EnsureGetMemberFloat("ItemSystemSaveY");
  SecondaryItemHighlightSprite =
      EnsureGetMemberSprite("SecondaryItemHighlightSprite");
  SecondaryItemHighlightX = EnsureGetMemberFloat("SecondaryItemHighlightX");
  SecondaryMenuPaddingY = EnsureGetMemberFloat("SecondaryMenuPaddingY");
  SecondaryMenuLoadOffsetY = EnsureGetMemberFloat("SecondaryMenuLoadOffsetY");
  SecondaryMenuLineX = EnsureGetMemberFloat("SecondaryMenuLineX");
  SecondaryMenuLoadLineY = EnsureGetMemberFloat("SecondaryMenuLoadLineY");
  SecondaryMenuLoadQuickLineY =
      EnsureGetMemberFloat("SecondaryMenuLoadQuickLineY");
  SecondaryMenuExtraClearY = EnsureGetMemberFloat("SecondaryMenuExtraClearY");
  SecondaryMenuExtraCGY = EnsureGetMemberFloat("SecondaryMenuExtraCGY");
  SecondaryMenuExtraSoundY = EnsureGetMemberFloat("SecondaryMenuExtraSoundY");
  SecondaryMenuExtraMovieY = EnsureGetMemberFloat("SecondaryMenuExtraMovieY");
  SecondaryMenuExtraTipsY = EnsureGetMemberFloat("SecondaryMenuExtraTipsY");
  SecondaryMenuExtraTrophyY = EnsureGetMemberFloat("SecondaryMenuExtraTrophyY");
  SecondaryMenuSystemConfigY =
      EnsureGetMemberFloat("SecondaryMenuSystemConfigY");
  SecondaryMenuSystemSaveY = EnsureGetMemberFloat("SecondaryMenuSystemSaveY");

  UI::CHLCC::TitleMenu* menu = new UI::CHLCC::TitleMenu();
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

  menu->SpinningCircleAnimation.LoopMode = ALM_Loop;
  menu->SpinningCircleAnimation.DurationIn = SpinningCircleAnimationDuration;
  menu->SpinningCircleAnimation.DurationOut = SpinningCircleAnimationDuration;

  UI::TitleMenuPtr = menu;
}

}  // namespace TitleMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto