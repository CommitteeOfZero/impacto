#include "titlemenu.h"
#include "../../../log.h"
#include "../../profile_internal.h"

#include "../../ui/titlemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/cclcc/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TitleMenu {

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  MainBackgroundSprite = EnsureGetMember<Sprite>("MainBackgroundSprite");
  CopyrightTextSprite = EnsureGetMember<Sprite>("CopyrightTextSprite");
  OverlaySprite = EnsureGetMember<Sprite>("OverlaySprite");
  SmokeSprite = EnsureGetMember<Sprite>("SmokeSprite");
  MenuSprite = EnsureGetMember<Sprite>("MenuSprite");
  ItemHighlightSprite = EnsureGetMember<Sprite>("ItemHighlightSprite");
  LoadSprite = EnsureGetMember<Sprite>("LoadSprite");
  LoadHighlightSprite = EnsureGetMember<Sprite>("LoadHighlightSprite");
  QuickLoadSprite = EnsureGetMember<Sprite>("QuickLoadSprite");
  QuickLoadHighlightSprite =
      EnsureGetMember<Sprite>("QuickLoadHighlightSprite");
  TipsSprite = EnsureGetMember<Sprite>("TipsSprite");
  TipsHighlightSprite = EnsureGetMember<Sprite>("TipsHighlightSprite");
  LibrarySprite = EnsureGetMember<Sprite>("LibrarySprite");
  LibraryHighlightSprite = EnsureGetMember<Sprite>("LibraryHighlightSprite");
  EndingListSprite = EnsureGetMember<Sprite>("EndingListSprite");
  EndingListHighlightSprite =
      EnsureGetMember<Sprite>("EndingListHighlightSprite");

  PrimaryFadeInDuration = EnsureGetMember<float>("PrimaryFadeInDuration");
  PrimaryFadeOutDuration = EnsureGetMember<float>("PrimaryFadeOutDuration");
  SecondaryFadeInDuration = EnsureGetMember<float>("SecondaryFadeInDuration");
  SecondaryFadeOutDuration = EnsureGetMember<float>("SecondaryFadeOutDuration");
  CopyrightTextX = EnsureGetMember<float>("CopyrightTextX");
  CopyrightTextY = EnsureGetMember<float>("CopyrightTextY");
  SmokeOpacityNormal = EnsureGetMember<float>("SmokeOpacityNormal");
  SmokeX = EnsureGetMember<float>("SmokeX");
  SmokeY = EnsureGetMember<float>("SmokeY");
  SmokeBoundsX = EnsureGetMember<float>("SmokeBoundsX");
  SmokeBoundsY = EnsureGetMember<float>("SmokeBoundsY");
  SmokeBoundsWidth = EnsureGetMember<float>("SmokeBoundsWidth");
  SmokeBoundsHeight = EnsureGetMember<float>("SmokeBoundsHeight");
  SmokeAnimationBoundsXOffset =
      EnsureGetMember<float>("SmokeAnimationBoundsXOffset");
  SmokeAnimationBoundsXMax = EnsureGetMember<float>("SmokeAnimationBoundsXMax");
  SmokeAnimationDurationIn = EnsureGetMember<float>("SmokeAnimationDurationIn");
  SmokeAnimationDurationOut =
      EnsureGetMember<float>("SmokeAnimationDurationOut");
  MenuX = EnsureGetMember<float>("MenuX");
  MenuY = EnsureGetMember<float>("MenuY");
  ItemHighlightOffsetX = EnsureGetMember<float>("ItemHighlightOffsetX");
  ItemHighlightOffsetY = EnsureGetMember<float>("ItemHighlightOffsetY");
  ItemPadding = EnsureGetMember<float>("ItemPadding");
  ItemYBase = EnsureGetMember<float>("ItemYBase");
  SecondaryFirstItemHighlightOffsetX =
      EnsureGetMember<float>("SecondaryFirstItemHighlightOffsetX");
  SecondarySecondItemHighlightOffsetX =
      EnsureGetMember<float>("SecondarySecondItemHighlightOffsetX");
  SecondaryThirdItemHighlightOffsetX =
      EnsureGetMember<float>("SecondaryThirdItemHighlightOffsetX");
  ItemHighlightPointerSprite =
      EnsureGetMember<Sprite>("ItemHighlightPointerSprite");
  ItemHighlightPointerY = EnsureGetMember<float>("ItemHighlightPointerY");

  TitleAnimationDurationIn = EnsureGetMember<float>("TitleAnimationDurationIn");
  TitleAnimationDurationOut =
      EnsureGetMember<float>("TitleAnimationDurationOut");
  TitleAnimationStartFrame = EnsureGetMember<int>("TitleAnimationStartFrame");
  TitleAnimationFrameCount = EnsureGetMember<int>("TitleAnimationFrameCount");
  TitleAnimationFileId = EnsureGetMember<int>("TitleAnimationFileId");

  ChoiceBlinkAnimationDurationIn =
      EnsureGetMember<float>("ChoiceBlinkAnimationDurationIn");
  SlideItemsAnimationDurationIn =
      EnsureGetMember<float>("SlideItemsAnimationDurationIn");
  SlideItemsAnimationDurationOut =
      EnsureGetMember<float>("SlideItemsAnimationDurationOut");
  HighlightAnimationDurationIn =
      EnsureGetMember<float>("HighlightAnimationDurationIn");
  HighlightAnimationDurationOut =
      EnsureGetMember<float>("HighlightAnimationDurationOut");
  ExtraDisabledTint = EnsureGetMember<uint32_t>("ExtraDisabledTint");

  UI::CCLCC::TitleMenu* menu = new UI::CCLCC::TitleMenu();
  menu->PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  menu->PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  menu->PressToStartAnimation.LoopMode = AnimationLoopMode::ReverseDirection;

  menu->PrimaryFadeAnimation.DurationIn = PrimaryFadeInDuration;
  menu->PrimaryFadeAnimation.DurationOut = PrimaryFadeOutDuration;
  menu->SecondaryFadeAnimation.DurationIn = SecondaryFadeInDuration;
  menu->SecondaryFadeAnimation.DurationOut = SecondaryFadeOutDuration;

  menu->SmokeAnimation.LoopMode = AnimationLoopMode::Loop;
  menu->SmokeAnimation.DurationIn = SmokeAnimationDurationIn;
  menu->SmokeAnimation.DurationOut = SmokeAnimationDurationOut;

  menu->TitleAnimation.DurationIn = TitleAnimationDurationIn;
  menu->TitleAnimation.DurationOut = TitleAnimationDurationOut;

  menu->TitleAnimationSprite.MountPoint = "system";
  menu->TitleAnimationSprite.LoadAsync(TitleAnimationFileId);

  UI::TitleMenuPtr = menu;

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto