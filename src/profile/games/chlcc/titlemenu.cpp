#include "titlemenu.h"
#include "../../../log.h"
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

  GetMemberSpriteArray(IntroHighlightSprites, IntroHighlightCount,
                       "IntroHighlightSprites");

  GetMemberFloatArray(IntroHighlightPositions, IntroHighlightCount,
                      "IntroHighlightPositions");

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
  menu->SpinningCircleAnimation.SetDuration(SpinningCircleAnimationDuration);

  IntroBackgroundSprite = EnsureGetMemberSprite("IntroBackgroundSprite");

  menu->IntroPanningAnimation.SetDuration(
      EnsureGetMemberFloat("IntroPanningAnimationDuration"));
  menu->IntroAfterPanningWaitAnimation.SetDuration(
      EnsureGetMemberFloat("IntroAfterPanningWaitDuration"));

  IntroBouncingStarSprite = EnsureGetMemberSprite("IntroBouncingStarSprite");

  menu->IntroStarBounceAnimation.SetDuration(
      EnsureGetMemberFloat("IntroBouncingStarAnimationDuration"));

  IntroExplodingStarSprite = EnsureGetMemberSprite("IntroExplodingStarSprite");
  IntroExplodingStarAnimationDistance =
      EnsureGetMemberFloat("IntroExplodingStarAnimationDistance");

  menu->IntroExplodingStarRotationAnimation.LoopMode = AnimationLoopMode::Loop;
  menu->IntroExplodingStarRotationAnimation.SetDuration(
      EnsureGetMemberFloat("IntroExplodingStarAnimationRotationDuration"));

  menu->IntroExplodingStarAnimation.SetDuration(
      EnsureGetMemberFloat("IntroExplodingStarAnimationDuration"));

  IntroFallingStarSprite = EnsureGetMemberSprite("IntroFallingStarSprite");
  IntroFallingStarsAnimationDistance =
      EnsureGetMemberFloat("IntroFallingStarsAnimationDistance");
  IntroFallingStarsAnimationDirection = glm::normalize(
      EnsureGetMemberVec2("IntroFallingStarsAnimationDirection"));
  menu->IntroFallingStarsAnimation.SetDuration(
      EnsureGetMemberFloat("IntroFallingStarsAnimationDuration"));

  menu->IntroFallingStarsRotationAnimation.LoopMode = AnimationLoopMode::Loop;
  menu->IntroFallingStarsRotationAnimation.SetDuration(
      EnsureGetMemberFloat("IntroFallingStarsAnimationRotationDuration"));

  menu->IntroAnimation.AddAnimation(&menu->IntroPanningAnimation);
  menu->IntroAnimation.AddAnimation(&menu->IntroAfterPanningWaitAnimation);
  menu->IntroAnimation.AddAnimation(&menu->IntroStarBounceAnimation);

  float explodingStarAnimationTime = menu->IntroAnimation.DurationIn;
  menu->IntroAnimation.AddAnimation(&menu->IntroExplodingStarAnimation);
  menu->IntroAnimation.AddAnimation(
      &menu->IntroExplodingStarRotationAnimation, explodingStarAnimationTime,
      menu->IntroExplodingStarAnimation.DurationIn);

  float fallingStarAnimationTime = menu->IntroAnimation.DurationIn;
  menu->IntroAnimation.AddAnimation(&menu->IntroFallingStarsAnimation);
  menu->IntroAnimation.AddAnimation(
      &menu->IntroFallingStarsRotationAnimation, fallingStarAnimationTime,
      menu->IntroFallingStarsAnimation.DurationIn);

  UI::TitleMenuPtr = menu;

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto