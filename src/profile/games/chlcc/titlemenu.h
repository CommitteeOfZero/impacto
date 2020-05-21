#pragma once

#include "../../../hud/titlemenu.h"
#include "../../../spritesheet.h"
#include "../../../games/chlcc/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TitleMenu {

Impacto::TitleMenu::TitleMenuBase* Configure();

extern Sprite BackgroundSprite;
extern Sprite DelusionADVUnderSprite;
extern Sprite DelusionADVSprite;
extern Sprite SeiraUnderSprite;
extern Sprite SeiraSprite;
extern Sprite CHLogoSprite;
extern Sprite LCCLogoUnderSprite;
extern Sprite ChuLeftLogoSprite;
extern Sprite ChuRightLogoSprite;
extern Sprite LoveLogoSprite;
extern Sprite StarLogoSprite;
extern Sprite ExclMarkLogoSprite;
extern Sprite CopyrightTextSprite;
extern Sprite SpinningCircleSprite;
extern Sprite ItemHighlightSprite;
extern Sprite ItemLoadLineSprite;
extern Sprite ItemLoadQuickLineSprite;
extern Sprite ItemLoadQuickSprite;
extern Sprite ItemLoadSprite;
extern Sprite ItemLoadQuickHighlightedSprite;
extern Sprite ItemLoadHighlightedSprite;
extern Sprite SecondaryItemHighlightSprite;

extern float DelusionADVUnderX;
extern float DelusionADVUnderY;
extern float DelusionADVX;
extern float DelusionADVY;
extern float SeiraUnderX;
extern float SeiraUnderY;
extern float SeiraX;
extern float SeiraY;
extern float CHLogoX;
extern float CHLogoY;
extern float LCCLogoUnderX;
extern float LCCLogoUnderY;
extern float ChuLeftLogoX;
extern float ChuLeftLogoY;
extern float ChuRightLogoX;
extern float ChuRightLogoY;
extern float LoveLogoX;
extern float LoveLogoY;
extern float StarLogoX;
extern float StarLogoY;
extern float ExclMarkLogoX;
extern float ExclMarkLogoY;
extern float CopyrightTextX;
extern float CopyrightTextY;
extern float SpinningCircleX;
extern float SpinningCircleY;
extern float SpinningCircleAnimationDuration;
extern float ItemHighlightOffsetX;
extern float ItemHighlightOffsetY;
extern float ItemPadding;
extern float ItemYBase;
extern float ItemFadeInDuration;
extern float ItemFadeOutDuration;
extern float ItemLoadX;
extern float ItemLoadY;
extern float ItemLoadQuickY;
extern float SecondaryItemHighlightX;
extern float SecondaryMenuPaddingY;
extern float SecondaryMenuLoadOffsetY;
extern float SecondaryMenuLoadLineX;
extern float SecondaryMenuLoadLineY;
extern float SecondaryMenuLoadQuickLineY;

}  // namespace TitleMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto