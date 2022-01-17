#pragma once

#include "../../../spritesheet.h"
#include "../../../games/cclcc/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TitleMenu {

extern Sprite BackgroundSprite;
extern Sprite MainBackgroundSprite;
extern Sprite CopyrightTextSprite;
extern Sprite OverlaySprite;
extern Sprite SmokeSprite;
extern Sprite MenuSprite;
extern Sprite ItemHighlightSprite;
extern Sprite ItemHighlightPointerSprite;
extern Sprite LoadSprite;
extern Sprite LoadHighlightSprite;
extern Sprite QuickLoadSprite;
extern Sprite QuickLoadHighlightSprite;
extern Sprite TipsSprite;
extern Sprite TipsHighlightSprite;
extern Sprite LibrarySprite;
extern Sprite LibraryHighlightSprite;
extern Sprite EndingListSprite;
extern Sprite EndingListHighlightSprite;

extern float ItemFadeInDuration;
extern float ItemFadeOutDuration;
extern float SecondaryItemFadeInDuration;
extern float SecondaryItemFadeOutDuration;
extern float PrimaryFadeInDuration;
extern float PrimaryFadeOutDuration;
extern float SecondaryFadeInDuration;
extern float SecondaryFadeOutDuration;
extern float CopyrightTextX;
extern float CopyrightTextY;
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
extern float MenuX;
extern float MenuY;
extern float ItemHighlightOffsetX;
extern float ItemHighlightOffsetY;
extern float ItemPadding;
extern float ItemYBase;
extern float SecondaryFirstItemHighlightOffsetX;
extern float SecondarySecondItemHighlightOffsetX;
extern float SecondaryThirdItemHighlightOffsetX;
extern float ItemHighlightPointerY;

void Configure();

}  // namespace TitleMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto