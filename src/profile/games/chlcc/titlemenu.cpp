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
  SpinningCircleFlashingAnimationDuration =
      EnsureGetMember<float>("SpinningCircleFlashingAnimationDuration");
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

  IntroBackgroundSprite = EnsureGetMember<Sprite>("IntroBackgroundSprite");
  IntroPanningAnimationDuration =
      EnsureGetMember<float>("IntroPanningAnimationDuration");

  IntroBouncingStarSprite = EnsureGetMember<Sprite>("IntroBouncingStarSprite");
  IntroStarBounceAnimationDuration =
      EnsureGetMember<float>("IntroBouncingStarAnimationDuration");

  IntroExplodingStarSprite =
      EnsureGetMember<Sprite>("IntroExplodingStarSprite");
  IntroExplodingStarAnimationDistance =
      EnsureGetMember<float>("IntroExplodingStarAnimationDistance");

  IntroExplodingStarRotationAnimationDuration =
      EnsureGetMember<float>("IntroExplodingStarAnimationRotationDuration");

  IntroExplodingStarAnimationDuration =
      EnsureGetMember<float>("IntroExplodingStarAnimationDuration");

  IntroFallingStarSprite = EnsureGetMember<Sprite>("IntroFallingStarSprite");
  IntroFallingStarsAnimationDistance =
      EnsureGetMember<float>("IntroFallingStarsAnimationDistance");
  IntroFallingStarsAnimationDirection = glm::normalize(
      EnsureGetMember<glm::vec2>("IntroFallingStarsAnimationDirection"));
  IntroFallingStarsAnimationDuration =
      EnsureGetMember<float>("IntroFallingStarsAnimationDuration");

  IntroFallingStarsRotationAnimationDuration =
      EnsureGetMember<float>("IntroFallingStarsAnimationRotationDuration");

  IntroCHLogoFadeAnimationStartY =
      EnsureGetMember<float>("IntroCHLogoFadeAnimationStartY");
  IntroLogoFadeAnimationDuration =
      EnsureGetMember<float>("IntroCHLogoFadeAnimationDuration");

  IntroLCCLogoAnimationDuration =
      EnsureGetMember<float>("IntroLCCLogoAnimationDuration");

  IntroLogoStarHighlightAnimationDuration =
      EnsureGetMember<float>("IntroLogoStarHighlightAnimationDuration");
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
  IntroDelusionADVAnimationDuration =
      EnsureGetMember<float>("IntroDelusionADVAnimationDuration");

  SeiraPopoutOffset = EnsureGetMember<glm::vec2>("SeiraPopoutOffset");
  IntroSeiraAnimationDuration =
      EnsureGetMember<float>("IntroSeiraAnimationDuration");

  IntroDelusionADVHighlightAnimationDuration =
      EnsureGetMember<float>("IntroDelusionADVHighlightAnimationDuration");

  IntroLogoPopOutOffset = EnsureGetMember<glm::vec2>("IntroLogoPopOutOffset");
  IntroLogoPopOutAnimationDuration =
      EnsureGetMember<float>("IntroLogoPopOutAnimationDuration");
  IntroLogoPopOutAnimationDelay =
      EnsureGetMember<float>("IntroLogoPopOutAnimationDelay");

  IntroCopyrightAnimationDuration =
      EnsureGetMember<float>("IntroCopyrightAnimationDuration");

  IntroAfterPanningWaitDuration =
      EnsureGetMember<float>("IntroAfterPanningWaitDuration");

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

  UI::TitleMenuPtr = menu;

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto