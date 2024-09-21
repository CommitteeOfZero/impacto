#pragma once

#include "../../../spritesheet.h"
#include "../../../games/cclcc/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TitleMenu {

inline Sprite BackgroundSprite;
inline Sprite MainBackgroundSprite;
inline Sprite CopyrightTextSprite;
inline Sprite OverlaySprite;
inline Sprite SmokeSprite;
inline Sprite MenuSprite;
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

inline float PrimaryFadeInDuration;
inline float PrimaryFadeOutDuration;
inline float SecondaryFadeInDuration;
inline float SecondaryFadeOutDuration;
inline float CopyrightTextX;
inline float CopyrightTextY;
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
inline float MenuX;
inline float MenuY;
inline float ItemHighlightOffsetX;
inline float ItemHighlightOffsetY;
inline float ItemPadding;
inline float ItemYBase;
inline float SecondaryFirstItemHighlightOffsetX;
inline float SecondarySecondItemHighlightOffsetX;
inline float SecondaryThirdItemHighlightOffsetX;
inline float ItemHighlightPointerY;
inline float TitleAnimationDurationIn;
inline float TitleAnimationDurationOut;
inline float ChoiceBlinkAnimationDuration;
inline float SlideItemsAnimationDurationIn;
inline float SlideItemsAnimationDurationOut;

inline int TitleAnimationStartFrame;
inline int TitleAnimationFrameCount;
inline int TitleAnimationFileId;

void Configure();

}  // namespace TitleMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto