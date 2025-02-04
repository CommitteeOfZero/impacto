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
  DelusionADVUnderPosition =
      EnsureGetMember<glm::vec2>("DelusionADVUnderPosition");
  DelusionADVSprite = EnsureGetMember<Sprite>("DelusionADVSprite");
  DelusionADVPosition = EnsureGetMember<glm::vec2>("DelusionADVPosition");
  SeiraUnderSprite = EnsureGetMember<Sprite>("SeiraUnderSprite");
  SeiraUnderPosition = EnsureGetMember<glm::vec2>("SeiraUnderPosition");
  SeiraSprite = EnsureGetMember<Sprite>("SeiraSprite");
  SeiraPosition = EnsureGetMember<glm::vec2>("SeiraPosition");
  CHLogoSprite = EnsureGetMember<Sprite>("CHLogoSprite");
  CHLogoPosition = EnsureGetMember<glm::vec2>("CHLogoPosition");
  LCCLogoUnderSprite = EnsureGetMember<Sprite>("LCCLogoUnderSprite");
  LCCLogoUnderPosition = EnsureGetMember<glm::vec2>("LCCLogoUnderPosition");
  ChuLeftLogoSprite = EnsureGetMember<Sprite>("ChuLeftLogoSprite");
  ChuLeftLogoPosition = EnsureGetMember<glm::vec2>("ChuLeftLogoPosition");
  ChuRightLogoSprite = EnsureGetMember<Sprite>("ChuRightLogoSprite");
  ChuRightLogoPosition = EnsureGetMember<glm::vec2>("ChuRightLogoPosition");
  LoveLogoSprite = EnsureGetMember<Sprite>("LoveLogoSprite");
  LoveLogoPosition = EnsureGetMember<glm::vec2>("LoveLogoPosition");
  StarLogoSprite = EnsureGetMember<Sprite>("StarLogoSprite");
  StarLogoPosition = EnsureGetMember<glm::vec2>("StarLogoPosition");
  ExclMarkLogoSprite = EnsureGetMember<Sprite>("ExclMarkLogoSprite");
  ExclMarkLogoPosition = EnsureGetMember<glm::vec2>("ExclMarkLogoPosition");
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

  GetMemberArray<Sprite>(IntroHighlightSprites, IntroHighlightCount,
                         "IntroHighlightSprites");

  GetMemberArray<float>(IntroHighlightPositions, IntroHighlightCount,
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

  IntroBackgroundSprite = EnsureGetMember<Sprite>("IntroBackgroundSprite");

  menu->IntroPanningAnimation.SetDuration(
      EnsureGetMember<float>("IntroPanningAnimationDuration"));
  menu->IntroAfterPanningWaitAnimation.SetDuration(
      EnsureGetMember<float>("IntroAfterPanningWaitDuration"));

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
      EnsureGetMember<int>("DrawType"));

  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto