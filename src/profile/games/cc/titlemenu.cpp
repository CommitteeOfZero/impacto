#include "titlemenu.h"
#include "../../../log.h"
#include "../../profile_internal.h"

#include "../../ui/titlemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/cc/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace TitleMenu {

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  FenceSprite = EnsureGetMember<Sprite>("FenceSprite");
  CopyrightSprite = EnsureGetMember<Sprite>("CopyrightSprite");
  OverlaySprite = EnsureGetMember<Sprite>("OverlaySprite");
  SmokeSprite = EnsureGetMember<Sprite>("SmokeSprite");
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
  MenuSprite = EnsureGetMember<Sprite>("MenuSprite");

  PressToStartAnimFastDurationIn =
      EnsureGetMember<float>("PressToStartAnimFastDurationIn");
  PressToStartAnimFastDurationOut =
      EnsureGetMember<float>("PressToStartAnimFastDurationOut");
  BackgroundX = EnsureGetMember<float>("BackgroundX");
  BackgroundY = EnsureGetMember<float>("BackgroundY");
  BackgroundBoundsX = EnsureGetMember<float>("BackgroundBoundsX");
  BackgroundBoundsYNormal = EnsureGetMember<float>("BackgroundBoundsYNormal");
  BackgroundBoundsYTrue = EnsureGetMember<float>("BackgroundBoundsYTrue");
  BackgroundBoundsWidth = EnsureGetMember<float>("BackgroundBoundsWidth");
  BackgroundBoundsHeight = EnsureGetMember<float>("BackgroundBoundsHeight");
  FenceX = EnsureGetMember<float>("FenceX");
  FenceY = EnsureGetMember<float>("FenceY");
  FenceBoundsWidth = EnsureGetMember<float>("FenceBoundsWidth");
  FenceBoundsHeight = EnsureGetMember<float>("FenceBoundsHeight");
  FenceBoundsX = EnsureGetMember<float>("FenceBoundsX");
  FenceBoundsYNormal = EnsureGetMember<float>("FenceBoundsYNormal");
  FenceBoundsYTrue = EnsureGetMember<float>("FenceBoundsYTrue");
  CopyrightX = EnsureGetMember<float>("CopyrightX");
  CopyrightY = EnsureGetMember<float>("CopyrightY");
  CopyrightXMoveOffset = EnsureGetMember<float>("CopyrightXMoveOffset");
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
  MoveLeftAnimationDurationIn =
      EnsureGetMember<float>("MoveLeftAnimationDurationIn");
  MoveLeftAnimationDurationOut =
      EnsureGetMember<float>("MoveLeftAnimationDurationOut");
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
  MenuX = EnsureGetMember<float>("MenuX");
  MenuY = EnsureGetMember<float>("MenuY");

  UI::CC::TitleMenu* menu = new UI::CC::TitleMenu();
  menu->PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  menu->PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  menu->PressToStartAnimation.LoopMode = AnimationLoopMode::ReverseDirection;

  menu->SmokeAnimation.LoopMode = AnimationLoopMode::Loop;
  menu->SmokeAnimation.DurationIn = SmokeAnimationDurationIn;
  menu->SmokeAnimation.DurationOut = SmokeAnimationDurationOut;
  menu->MoveLeftAnimation.DurationIn = MoveLeftAnimationDurationIn;
  menu->MoveLeftAnimation.DurationOut = MoveLeftAnimationDurationOut;
  UI::TitleMenuPtr = menu;

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto