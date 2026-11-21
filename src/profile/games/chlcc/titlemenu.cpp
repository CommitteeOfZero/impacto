#include "titlemenu.h"
#include "../../../renderer/renderer.h"
#include "../../profile_internal.h"

#include "../../ui/titlemenu.h"
#include "../../game.h"
#include "../../patch.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/titlemenu.h"
#include "../../../games/chlcc/titlemenu_ps3.h"
#include "../../../games/chlcc/titlemenu_switch.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TitleMenu {

void Configure() {
  if (Patch::HasScriptedExitLogic) {
    ExitSprite = EnsureGetMember<Sprite>("ExitSprite");
    ExitHighlightSprite = EnsureGetMember<Sprite>("ExitHighlightSprite");
  }

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
  GetMemberArray<Sprite>(std::span(LineSprites, LineNum), "LineEntriesSprites");
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

  GetMemberArray<Sprite>(
      std::span(IntroHighlightSprites.data(), IntroHighlightCount),
      "IntroHighlightSprites");
  GetMemberArray<float>(
      std::span(IntroHighlightPositions.data(), IntroHighlightCount),
      "IntroHighlightPositions");

  GetMemberArray<Sprite>(std::span(LCCLogoSprites.data(), LCCLogoSpriteCount),
                         "LCCLogoSprites");
  GetMemberArray<glm::vec2>(
      std::span(LCCLogoPositions.data(), LCCLogoSpriteCount),
      "LCCLogoPositions");

  IntroBackgroundSprite = EnsureGetMember<Sprite>("IntroBackgroundSprite");
  IntroPanningAnimationDuration =
      EnsureGetMember<float>("IntroPanningAnimationDuration");

  IntroBouncingStarSprite = EnsureGetMember<Sprite>("IntroBouncingStarSprite");
  IntroBouncingStarBaseYOffset =
      EnsureGetMember<float>("IntroBouncingStarBaseYOffset");
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
  GetMemberArray<Sprite>(
      std::span(IntroDelusionADVSprites.data(), IntroDelusionADVSpriteCount),
      "IntroDelusionADVSprites");
  GetMemberArray<glm::vec2>(
      std::span(IntroDelusionADVPositions.data(), IntroDelusionADVSpriteCount),
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

  NewButtonId = EnsureGetMember<int>("NewButtonId");
  PrimLoadButtonId = EnsureGetMember<int>("PrimLoadButtonId");
  ExtraButtonId = EnsureGetMember<int>("ExtraButtonId");
  SystemButtonId = EnsureGetMember<int>("SystemButtonId");
  LoadButtonId = EnsureGetMember<int>("LoadButtonId");
  QLoadButtonId = EnsureGetMember<int>("QLoadButtonId");
  EndingListButtonId = EnsureGetMember<int>("EndingListButtonId");
  CGButtonId = EnsureGetMember<int>("CGButtonId");
  SoundButtonId = EnsureGetMember<int>("SoundButtonId");
  MovieButtonId = EnsureGetMember<int>("MovieButtonId");
  TipsButtonId = EnsureGetMember<int>("TipsButtonId");
  LockedTipsButtonId =
      TryGetMember<int>("LockedTipsButtonId").value_or(TipsButtonId);
  // PS3 specific
  ConfigButtonId = TryGetMember<int>("ConfigButtonId");
  SystemSaveButtonId = TryGetMember<int>("SystemSaveButtonId");
  TrophyButtonId = TryGetMember<int>("TrophyButtonId");
  LockedTrophyButtonId = TryGetMember<int>("LockedTrophyButtonId");
  // Switch specific
  CostumeButtonId = TryGetMember<int>("CostumeButtonId");
  HelpButtonId = TryGetMember<int>("HelpButtonId");
  // Patch dependent
  ExitButtonId = TryGetMember<int>("ExitButtonId");

  UI::CHLCC::TitleMenu* menu;
  if (Profile::TitleMenu::Type == UI::TitleMenuType::CHLCC_Switch) {
    menu = new UI::CHLCC_Switch::TitleMenu();
    auto drawType =
        EnsureGetMember<Impacto::Game::DrawComponentType>("DrawType");
    UI::Menus[drawType].push_back(menu);
  } else {
    // Setting TitleMenuPtr and not pushing to UI::Menus, so it will be drawn on
    // layer, required for proper draw order with restart mask
    menu = new UI::CHLCC_PS3::TitleMenu();
    UI::TitleMenuPtr = menu;
  }

  menu->PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  menu->PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  menu->PressToStartAnimation.LoopMode = AnimationLoopMode::ReverseDirection;

  menu->PrimaryFadeAnimation.DurationIn = PrimaryFadeInDuration;
  menu->PrimaryFadeAnimation.DurationOut = PrimaryFadeOutDuration;

  menu->SecondaryFadeAnimation.DurationIn = SecondaryFadeInDuration;
  menu->SecondaryFadeAnimation.DurationOut = SecondaryFadeOutDuration;

  menu->SpinningCircleAnimation.LoopMode = AnimationLoopMode::Loop;
  menu->SpinningCircleAnimation.SetDuration(SpinningCircleAnimationDuration);

  menu->SpinningCircleFlashingAnimation.LoopMode =
      AnimationLoopMode::ReverseDirection;
}

}  // namespace TitleMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto