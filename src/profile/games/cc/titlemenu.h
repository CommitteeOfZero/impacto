#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace TitleMenu {

void Configure();

extern Sprite BackgroundSprite;
extern Sprite CopyrightSprite;
extern Sprite FenceSprite;
extern Sprite OverlaySprite;
extern Sprite SmokeSprite;

extern float PressToStartAnimFastDurationIn;
extern float PressToStartAnimFastDurationOut;
extern float BackgroundX;
extern float BackgroundY;
extern float BackgroundBoundsX;
extern float BackgroundBoundsYNormal;
extern float BackgroundBoundsYTrue;
extern float BackgroundBoundsWidth;
extern float BackgroundBoundsHeight;
extern float FenceX;
extern float FenceY;
extern float FenceBoundsWidth;
extern float FenceBoundsHeight;
extern float FenceBoundsX;
extern float FenceBoundsYNormal;
extern float FenceBoundsYTrue;
extern float CopyrightX;
extern float CopyrightY;
extern float CopyrightXMoveOffset;
extern float SmokeOpacityNormal;
extern float SmokeX;
extern float SmokeY;
extern float SmokeBoundsX;
extern float SmokeBoundsY;
extern float SmokeBoundsWidth;
extern float SmokeBoundsHeight;
extern float SmokeAnimationBoundsXOffset;
extern float SmokeAnimationBoundsXMax;
extern float SmokeAnimationDurationIn;
extern float SmokeAnimationDurationOut;
extern float MoveLeftAnimationDurationIn;
extern float MoveLeftAnimationDurationOut;

}  // namespace TitleMenu
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto