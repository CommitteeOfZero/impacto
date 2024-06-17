#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace TitleMenu {

void Configure();

inline Sprite BackgroundSprite;
inline Sprite CopyrightSprite;
inline Sprite FenceSprite;
inline Sprite OverlaySprite;
inline Sprite SmokeSprite;
inline Sprite ItemHighlightSprite;
inline Sprite ItemHighlightPointerSprite;
inline Sprite LoadSprite;
inline Sprite LoadHighlightSprite;
inline Sprite QuickLoadSprite;
inline Sprite QuickLoadHighlightSprite;
inline Sprite TipsSprite;
inline Sprite TipsHighlightSprite;
inline Sprite LibrarySprite;
inline Sprite LibraryHighlightSprite;
inline Sprite EndingListSprite;
inline Sprite EndingListHighlightSprite;
inline Sprite MenuSprite;

inline float PressToStartAnimFastDurationIn;
inline float PressToStartAnimFastDurationOut;
inline float BackgroundX;
inline float BackgroundY;
inline float BackgroundBoundsX;
inline float BackgroundBoundsYNormal;
inline float BackgroundBoundsYTrue;
inline float BackgroundBoundsWidth;
inline float BackgroundBoundsHeight;
inline float FenceX;
inline float FenceY;
inline float FenceBoundsWidth;
inline float FenceBoundsHeight;
inline float FenceBoundsX;
inline float FenceBoundsYNormal;
inline float FenceBoundsYTrue;
inline float CopyrightX;
inline float CopyrightY;
inline float CopyrightXMoveOffset;
inline float SmokeOpacityNormal;
inline float SmokeX;
inline float SmokeY;
inline float SmokeBoundsX;
inline float SmokeBoundsY;
inline float SmokeBoundsWidth;
inline float SmokeBoundsHeight;
inline float SmokeAnimationBoundsXOffset;
inline float SmokeAnimationBoundsXMax;
inline float SmokeAnimationDurationIn;
inline float SmokeAnimationDurationOut;
inline float MoveLeftAnimationDurationIn;
inline float MoveLeftAnimationDurationOut;
inline float ItemHighlightOffsetX;
inline float ItemHighlightOffsetY;
inline float ItemPadding;
inline float ItemYBase;
inline float SecondaryFirstItemHighlightOffsetX;
inline float SecondarySecondItemHighlightOffsetX;
inline float SecondaryThirdItemHighlightOffsetX;
inline float ItemHighlightPointerY;
inline float MenuX;
inline float MenuY;

}  // namespace TitleMenu
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto