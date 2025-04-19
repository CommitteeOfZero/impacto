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
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  DelusionADVUnderSprite = EnsureGetMember<Sprite>("DelusionADVUnderSprite");
  DelusionADVSprite = EnsureGetMember<Sprite>("DelusionADVSprite");
  DelusionADVPosition = EnsureGetMember<glm::vec2>("DelusionADVPosition");
  DelusionADVPopoutOffset =
      EnsureGetMember<glm::vec2>("DelusionADVPopoutOffset");
  SeiraUnderSprite = EnsureGetMember<Sprite>("SeiraUnderSprite");
  SeiraUnderPosition = EnsureGetMember<glm::vec2>("SeiraUnderPosition");
  SeiraSprite = EnsureGetMember<Sprite>("SeiraSprite");
  SeiraPosition = EnsureGetMember<glm::vec2>("SeiraPosition");
  CHLogoSprite = EnsureGetMember<Sprite>("CHLogoSprite");
  CHLogoPosition = EnsureGetMember<glm::vec2>("CHLogoPosition");
  LCCLogoUnderSprite = EnsureGetMember<Sprite>("LCCLogoUnderSprite");
  LCCLogoUnderPosition = EnsureGetMember<glm::vec2>("LCCLogoUnderPosition");
  StarLogoSprite = EnsureGetMember<Sprite>("StarLogoSprite");
  StarLogoPosition = EnsureGetMember<glm::vec2>("StarLogoPosition");
  CopyrightTextSprite = EnsureGetMember<Sprite>("CopyrightTextSprite");
  CopyrightTextPosition = EnsureGetMember<glm::vec2>("CopyrightTextPosition");
  SpinningCircleSprite = EnsureGetMember<Sprite>("SpinningCircleSprite");
  SpinningCirclePosition = EnsureGetMember<glm::vec2>("SpinningCirclePosition");
  SpinningCircleAnimationDuration =
      EnsureGetMember<float>("SpinningCircleAnimationDuration");
  ItemHighlightSprite = EnsureGetMember<Sprite>("ItemHighlightSprite");
  ItemHighlightOffset = EnsureGetMember<glm::vec2>("ItemHighlightOffset");
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

  GetMemberArray<Sprite>(IntroHighlightSprites.data(), IntroHighlightCount,
                         "IntroHighlightSprites");
  GetMemberArray<float>(IntroHighlightPositions.data(), IntroHighlightCount,
                        "IntroHighlightPositions");

  GetMemberArray<Sprite>(LCCLogoSprites.data(), LCCLogoSpriteCount,
                         "LCCLogoSprites");
  GetMemberArray<glm::vec2>(LCCLogoPositions.data(), LCCLogoSpriteCount,
                            "LCCLogoPositions");

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

  menu->SpinningCircleFlashingAnimation.LoopMode =
      AnimationLoopMode::ReverseDirection;
  menu->SpinningCircleFlashingAnimation.SetDuration(
      EnsureGetMember<float>("SpinningCircleFlashingAnimationDuration"));

  IntroBackgroundSprite = EnsureGetMember<Sprite>("IntroBackgroundSprite");

  menu->IntroPanningAnimation.SetDuration(
      EnsureGetMember<float>("IntroPanningAnimationDuration"));

  IntroBouncingStarSprite = EnsureGetMember<Sprite>("IntroBouncingStarSprite");
  menu->IntroStarBounceAnimation.SetDuration(
      EnsureGetMember<float>("IntroBouncingStarAnimationDuration"));

  IntroExplodingStarSprite =
      EnsureGetMember<Sprite>("IntroExplodingStarSprite");
  IntroExplodingStarAnimationDistance =
      EnsureGetMember<float>("IntroExplodingStarAnimationDistance");

  menu->IntroExplodingStarRotationAnimation.LoopMode = AnimationLoopMode::Loop;
  menu->IntroExplodingStarRotationAnimation.SetDuration(
      EnsureGetMember<float>("IntroExplodingStarAnimationRotationDuration"));

  menu->IntroExplodingStarAnimation.SetDuration(
      EnsureGetMember<float>("IntroExplodingStarAnimationDuration"));

  IntroFallingStarSprite = EnsureGetMember<Sprite>("IntroFallingStarSprite");
  IntroFallingStarsAnimationDistance =
      EnsureGetMember<float>("IntroFallingStarsAnimationDistance");
  IntroFallingStarsAnimationDirection = glm::normalize(
      EnsureGetMember<glm::vec2>("IntroFallingStarsAnimationDirection"));
  menu->IntroFallingStarsAnimation.SetDuration(
      EnsureGetMember<float>("IntroFallingStarsAnimationDuration"));

  menu->IntroFallingStarsRotationAnimation.LoopMode = AnimationLoopMode::Loop;
  menu->IntroFallingStarsRotationAnimation.SetDuration(
      EnsureGetMember<float>("IntroFallingStarsAnimationRotationDuration"));

  IntroCHLogoFadeAnimationStartY =
      EnsureGetMember<float>("IntroCHLogoFadeAnimationStartY");
  menu->IntroLogoFadeAnimation.SetDuration(
      EnsureGetMember<float>("IntroCHLogoFadeAnimationDuration"));

  menu->IntroLCCLogoAnimation.SetDuration(
      EnsureGetMember<float>("IntroLCCLogoAnimationDuration"));

  menu->IntroLogoStarHighlightAnimation.SetDuration(
      EnsureGetMember<float>("IntroLogoStarHighlightAnimationDuration"));
  IntroLogoStarHighlightSprite =
      EnsureGetMember<Sprite>("IntroLogoStarHighlightSprite");
  IntroLogoStarHighlightPosition =
      EnsureGetMember<glm::vec2>("IntroLogoStarHighlightPosition");

  IntroDelusionADVSpriteCount =
      EnsureGetMember<int>("IntroDelusionADVSpriteCount");
  GetMemberArray<Sprite>(IntroDelusionADVSprites.data(),
                         IntroDelusionADVSpriteCount,
                         "IntroDelusionADVSprites");
  GetMemberArray<glm::vec2>(IntroDelusionADVPositions.data(),
                            IntroDelusionADVSpriteCount,
                            "IntroDelusionADVPositions");
  menu->IntroDelusionADVAnimation.SetDuration(
      EnsureGetMember<float>("IntroDelusionADVAnimationDuration"));

  SeiraPopoutOffset = EnsureGetMember<glm::vec2>("SeiraPopoutOffset");
  menu->IntroSeiraAnimation.SetDuration(
      EnsureGetMember<float>("IntroSeiraAnimationDuration"));

  menu->IntroDelusionADVHighlightAnimation.SetDuration(
      EnsureGetMember<float>("IntroDelusionADVHighlightAnimationDuration"));

  IntroLogoPopOutOffset = EnsureGetMember<glm::vec2>("IntroLogoPopOutOffset");
  menu->IntroLogoPopOutAnimation.SetDuration(
      EnsureGetMember<float>("IntroLogoPopOutAnimationDuration"));

  menu->IntroCopyrightAnimation.SetDuration(
      EnsureGetMember<float>("IntroCopyrightAnimationDuration"));

  menu->IntroAnimation.AddAnimation(&menu->IntroPanningAnimation);

  float starBounceTime =
      menu->IntroAnimation.DurationIn +
      EnsureGetMember<float>("IntroAfterPanningWaitDuration");
  menu->IntroAnimation.AddAnimation(&menu->IntroStarBounceAnimation,
                                    starBounceTime);

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

  menu->IntroAnimation.AddAnimation(&menu->IntroLogoFadeAnimation);

  float lccLogoAnimationTime = menu->IntroAnimation.DurationIn;
  menu->IntroAnimation.AddAnimation(&menu->IntroLCCLogoAnimation);
  menu->IntroAnimation.AddAnimation(&menu->IntroDelusionADVAnimation,
                                    lccLogoAnimationTime,
                                    menu->IntroDelusionADVAnimation.DurationIn);
  menu->IntroAnimation.AddAnimation(&menu->IntroLogoStarHighlightAnimation);

  float seiraAnimationTime = menu->IntroAnimation.DurationIn;
  menu->IntroAnimation.AddAnimation(&menu->IntroSeiraAnimation);
  menu->IntroAnimation.AddAnimation(
      &menu->IntroDelusionADVHighlightAnimation, seiraAnimationTime,
      menu->IntroDelusionADVHighlightAnimation.DurationIn);
  menu->IntroAnimation.AddAnimation(
      &menu->IntroLogoPopOutAnimation,
      seiraAnimationTime +
          EnsureGetMember<float>("IntroLogoPopOutAnimationDelay"),
      menu->IntroLogoPopOutAnimation.DurationIn);

  menu->IntroAnimation.AddAnimation(&menu->IntroCopyrightAnimation);

  UI::TitleMenuPtr = menu;

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto