#include "titlemenu.h"
#include "../../../log.h"
// #include "../../../window.h"
#include "../../../renderer/renderer.h"
#include "../../profile_internal.h"

#include "../../ui/titlemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TitleMenu {

void Configure() {
  IntroBackgroundSprite = EnsureGetMember<Sprite>("IntroBackgroundSprite");
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  DelusionADVUnderSprite = EnsureGetMember<Sprite>("DelusionADVUnderSprite");
  DelusionADVUnderX = EnsureGetMember<float>("DelusionADVUnderX");
  DelusionADVUnderY = EnsureGetMember<float>("DelusionADVUnderY");
  DelusionADVSprite = EnsureGetMember<Sprite>("DelusionADVSprite");
  DelusionADVX = EnsureGetMember<float>("DelusionADVX");
  DelusionADVY = EnsureGetMember<float>("DelusionADVY");
  SeiraUnderSprite = EnsureGetMember<Sprite>("SeiraUnderSprite");
  SeiraUnderX = EnsureGetMember<float>("SeiraUnderX");
  SeiraUnderY = EnsureGetMember<float>("SeiraUnderY");
  SeiraSprite = EnsureGetMember<Sprite>("SeiraSprite");
  SeiraX = EnsureGetMember<float>("SeiraX");
  SeiraY = EnsureGetMember<float>("SeiraY");
  CHLogoSprite = EnsureGetMember<Sprite>("CHLogoSprite");
  CHLogoX = EnsureGetMember<float>("CHLogoX");
  CHLogoY = EnsureGetMember<float>("CHLogoY");
  LCCLogoUnderSprite = EnsureGetMember<Sprite>("LCCLogoUnderSprite");
  LCCLogoUnderX = EnsureGetMember<float>("LCCLogoUnderX");
  LCCLogoUnderY = EnsureGetMember<float>("LCCLogoUnderY");
  ChuLeftLogoSprite = EnsureGetMember<Sprite>("ChuLeftLogoSprite");
  ChuLeftLogoX = EnsureGetMember<float>("ChuLeftLogoX");
  ChuLeftLogoY = EnsureGetMember<float>("ChuLeftLogoY");
  ChuRightLogoSprite = EnsureGetMember<Sprite>("ChuRightLogoSprite");
  ChuRightLogoX = EnsureGetMember<float>("ChuRightLogoX");
  ChuRightLogoY = EnsureGetMember<float>("ChuRightLogoY");
  LoveLogoSprite = EnsureGetMember<Sprite>("LoveLogoSprite");
  LoveLogoX = EnsureGetMember<float>("LoveLogoX");
  LoveLogoY = EnsureGetMember<float>("LoveLogoY");
  StarLogoSprite = EnsureGetMember<Sprite>("StarLogoSprite");
  StarLogoX = EnsureGetMember<float>("StarLogoX");
  StarLogoY = EnsureGetMember<float>("StarLogoY");
  ExclMarkLogoSprite = EnsureGetMember<Sprite>("ExclMarkLogoSprite");
  ExclMarkLogoX = EnsureGetMember<float>("ExclMarkLogoX");
  ExclMarkLogoY = EnsureGetMember<float>("ExclMarkLogoY");
  CopyrightTextSprite = EnsureGetMember<Sprite>("CopyrightTextSprite");
  CopyrightTextX = EnsureGetMember<float>("CopyrightTextX");
  CopyrightTextY = EnsureGetMember<float>("CopyrightTextY");
  SpinningCircleSprite = EnsureGetMember<Sprite>("SpinningCircleSprite");
  SpinningCircleX = EnsureGetMember<float>("SpinningCircleX");
  SpinningCircleY = EnsureGetMember<float>("SpinningCircleY");
  SpinningCircleAnimationDuration =
      EnsureGetMember<float>("SpinningCircleAnimationDuration");
  ItemHighlightSprite = EnsureGetMember<Sprite>("ItemHighlightSprite");
  ItemHighlightOffsetX = EnsureGetMember<float>("ItemHighlightOffsetX");
  ItemHighlightOffsetY = EnsureGetMember<float>("ItemHighlightOffsetY");
  ItemPadding = EnsureGetMember<float>("ItemPadding");
  ItemYBase = EnsureGetMember<float>("ItemYBase");
  ItemFadeInDuration = EnsureGetMember<float>("ItemFadeInDuration");
  ItemFadeOutDuration = EnsureGetMember<float>("ItemFadeOutDuration");
  SecondaryItemFadeInDuration =
      EnsureGetMember<float>("SecondaryItemFadeInDuration");
  SecondaryItemFadeOutDuration =
      EnsureGetMember<float>("SecondaryItemFadeOutDuration");
  PrimaryFadeInDuration = EnsureGetMember<float>("PrimaryFadeInDuration");
  PrimaryFadeOutDuration = EnsureGetMember<float>("PrimaryFadeOutDuration");
  SecondaryFadeInDuration = EnsureGetMember<float>("SecondaryFadeInDuration");
  SecondaryFadeOutDuration = EnsureGetMember<float>("SecondaryFadeOutDuration");
  LineNum = EnsureGetMember<int>("LineNum");
  GetMemberArray<Sprite>(LineSprites, LineNum, "LineEntriesSprites");
  ItemLoadQuickSprite = EnsureGetMember<Sprite>("ItemLoadQuickSprite");
  SecondaryItemX = EnsureGetMember<float>("SecondaryItemX");
  ItemLoadY = EnsureGetMember<float>("ItemLoadY");
  ItemLoadQuickY = EnsureGetMember<float>("ItemLoadQuickY");
  ItemLoadSprite = EnsureGetMember<Sprite>("ItemLoadSprite");
  ItemLoadQuickHighlightedSprite =
      EnsureGetMember<Sprite>("ItemLoadQuickHighlightedSprite");
  ItemLoadHighlightedSprite =
      EnsureGetMember<Sprite>("ItemLoadHighlightedSprite");
  ItemClearListY = EnsureGetMember<float>("ItemClearListY");
  ItemCGLibraryY = EnsureGetMember<float>("ItemCGLibraryY");
  ItemSoundLibraryY = EnsureGetMember<float>("ItemSoundLibraryY");
  ItemMovieLibraryY = EnsureGetMember<float>("ItemMovieLibraryY");
  ItemTipsY = EnsureGetMember<float>("ItemTipsY");
  ItemTrophyY = EnsureGetMember<float>("ItemTrophyY");
  ItemConfigY = EnsureGetMember<float>("ItemConfigY");
  ItemSystemSaveY = EnsureGetMember<float>("ItemSystemSaveY");
  SecondaryItemHighlightSprite =
      EnsureGetMember<Sprite>("SecondaryItemHighlightSprite");
  SecondaryItemHighlightX = EnsureGetMember<float>("SecondaryItemHighlightX");
  SecondaryMenuPaddingY = EnsureGetMember<float>("SecondaryMenuPaddingY");
  SecondaryMenuLoadOffsetY = EnsureGetMember<float>("SecondaryMenuLoadOffsetY");
  SecondaryMenuLineX = EnsureGetMember<float>("SecondaryMenuLineX");
  SecondaryMenuLoadLineY = EnsureGetMember<float>("SecondaryMenuLoadLineY");
  SecondaryMenuLoadQuickLineY =
      EnsureGetMember<float>("SecondaryMenuLoadQuickLineY");
  SecondaryMenuExtraClearY = EnsureGetMember<float>("SecondaryMenuExtraClearY");
  SecondaryMenuExtraCGY = EnsureGetMember<float>("SecondaryMenuExtraCGY");
  SecondaryMenuExtraSoundY = EnsureGetMember<float>("SecondaryMenuExtraSoundY");
  SecondaryMenuExtraMovieY = EnsureGetMember<float>("SecondaryMenuExtraMovieY");
  SecondaryMenuExtraTipsY = EnsureGetMember<float>("SecondaryMenuExtraTipsY");
  SecondaryMenuExtraTrophyY =
      EnsureGetMember<float>("SecondaryMenuExtraTrophyY");
  SecondaryMenuSystemConfigY =
      EnsureGetMember<float>("SecondaryMenuSystemConfigY");
  SecondaryMenuSystemSaveY = EnsureGetMember<float>("SecondaryMenuSystemSaveY");

  UI::CHLCC::TitleMenu* menu = new UI::CHLCC::TitleMenu();
  menu->PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  menu->PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  menu->PressToStartAnimation.LoopMode = AnimationLoopMode::ReverseDirection;

  menu->ItemsFadeInAnimation.DurationIn = ItemFadeInDuration;
  menu->ItemsFadeInAnimation.DurationOut = ItemFadeOutDuration;

  menu->SecondaryItemsFadeInAnimation.DurationIn = SecondaryItemFadeInDuration;
  menu->SecondaryItemsFadeInAnimation.DurationOut =
      SecondaryItemFadeOutDuration;

  menu->SpinningCircleAnimation.LoopMode = AnimationLoopMode::Loop;
  menu->SpinningCircleAnimation.DurationIn = SpinningCircleAnimationDuration;
  menu->SpinningCircleAnimation.DurationOut = SpinningCircleAnimationDuration;

  UI::TitleMenuPtr = menu;

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto