#include "titlemenu.h"
#include "../../../log.h"
#include "../../../window.h"
#include "../../profile_internal.h"

#include "../../ui/titlemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/cc/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace TitleMenu {

Sprite BackgroundSprite;
Sprite CopyrightSprite;
Sprite FenceSprite;
Sprite OverlaySprite;
Sprite SmokeSprite;

float PressToStartAnimFastDurationIn;
float PressToStartAnimFastDurationOut;
float BackgroundX;
float BackgroundY;
float BackgroundBoundsX;
float BackgroundBoundsYNormal;
float BackgroundBoundsYTrue;
float BackgroundBoundsWidth;
float BackgroundBoundsHeight;
float FenceX;
float FenceY;
float FenceBoundsWidth;
float FenceBoundsHeight;
float FenceBoundsX;
float FenceBoundsYNormal;
float FenceBoundsYTrue;
float CopyrightX;
float CopyrightY;
float CopyrightXMoveOffset;
float SmokeOpacityNormal;
float SmokeX;
float SmokeY;
float SmokeBoundsX;
float SmokeBoundsY;
float SmokeBoundsWidth;
float SmokeBoundsHeight;
float SmokeAnimationBoundsXOffset;
float SmokeAnimationBoundsXMax;
float SmokeAnimationDurationIn;
float SmokeAnimationDurationOut;
float MoveLeftAnimationDurationIn;
float MoveLeftAnimationDurationOut;

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  FenceSprite = EnsureGetMemberSprite("FenceSprite");
  CopyrightSprite = EnsureGetMemberSprite("CopyrightSprite");
  OverlaySprite = EnsureGetMemberSprite("OverlaySprite");
  SmokeSprite = EnsureGetMemberSprite("SmokeSprite");

  PressToStartAnimFastDurationIn =
      EnsureGetMemberFloat("PressToStartAnimFastDurationIn");
  PressToStartAnimFastDurationOut =
      EnsureGetMemberFloat("PressToStartAnimFastDurationOut");
  BackgroundX = EnsureGetMemberFloat("BackgroundX");
  BackgroundY = EnsureGetMemberFloat("BackgroundY");
  BackgroundBoundsX = EnsureGetMemberFloat("BackgroundBoundsX");
  BackgroundBoundsYNormal = EnsureGetMemberFloat("BackgroundBoundsYNormal");
  BackgroundBoundsYTrue = EnsureGetMemberFloat("BackgroundBoundsYTrue");
  BackgroundBoundsWidth = EnsureGetMemberFloat("BackgroundBoundsWidth");
  BackgroundBoundsHeight = EnsureGetMemberFloat("BackgroundBoundsHeight");
  FenceX = EnsureGetMemberFloat("FenceX");
  FenceY = EnsureGetMemberFloat("FenceY");
  FenceBoundsWidth = EnsureGetMemberFloat("FenceBoundsWidth");
  FenceBoundsHeight = EnsureGetMemberFloat("FenceBoundsHeight");
  FenceBoundsX = EnsureGetMemberFloat("FenceBoundsX");
  FenceBoundsYNormal = EnsureGetMemberFloat("FenceBoundsYNormal");
  FenceBoundsYTrue = EnsureGetMemberFloat("FenceBoundsYTrue");
  CopyrightX = EnsureGetMemberFloat("CopyrightX");
  CopyrightY = EnsureGetMemberFloat("CopyrightY");
  CopyrightXMoveOffset = EnsureGetMemberFloat("CopyrightXMoveOffset");
  SmokeOpacityNormal = EnsureGetMemberFloat("SmokeOpacityNormal");
  SmokeX = EnsureGetMemberFloat("SmokeX");
  SmokeY = EnsureGetMemberFloat("SmokeY");
  SmokeBoundsX = EnsureGetMemberFloat("SmokeBoundsX");
  SmokeBoundsY = EnsureGetMemberFloat("SmokeBoundsY");
  SmokeBoundsWidth = EnsureGetMemberFloat("SmokeBoundsWidth");
  SmokeBoundsHeight = EnsureGetMemberFloat("SmokeBoundsHeight");
  SmokeAnimationBoundsXOffset =
      EnsureGetMemberFloat("SmokeAnimationBoundsXOffset");
  SmokeAnimationBoundsXMax = EnsureGetMemberFloat("SmokeAnimationBoundsXMax");
  SmokeAnimationDurationIn = EnsureGetMemberFloat("SmokeAnimationDurationIn");
  SmokeAnimationDurationOut = EnsureGetMemberFloat("SmokeAnimationDurationOut");
  MoveLeftAnimationDurationIn =
      EnsureGetMemberFloat("MoveLeftAnimationDurationIn");
  MoveLeftAnimationDurationOut =
      EnsureGetMemberFloat("MoveLeftAnimationDurationOut");

  UI::CC::TitleMenu* menu = new UI::CC::TitleMenu();
  menu->PressToStartAnimation.DurationIn =
      Profile::TitleMenu::PressToStartAnimDurationIn;
  menu->PressToStartAnimation.DurationOut =
      Profile::TitleMenu::PressToStartAnimDurationOut;
  menu->PressToStartAnimation.LoopMode = ALM_ReverseDirection;
  menu->SmokeAnimation.LoopMode = ALM_Loop;
  menu->SmokeAnimation.DurationIn = SmokeAnimationDurationIn;
  menu->SmokeAnimation.DurationOut = SmokeAnimationDurationOut;
  menu->MoveLeftAnimation.DurationIn = MoveLeftAnimationDurationIn;
  menu->MoveLeftAnimation.DurationOut = MoveLeftAnimationDurationOut;
  UI::TitleMenuPtr = menu;
}

}  // namespace TitleMenu
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto