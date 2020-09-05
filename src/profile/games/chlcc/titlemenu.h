#pragma once

#include "../../../spritesheet.h"
#include "../../../games/chlcc/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TitleMenu {

void Configure();

static int const LineEntriesNumMax = 32;

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
extern Sprite LineSprites[LineEntriesNumMax];
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
extern float SecondaryItemFadeInDuration;
extern float SecondaryItemFadeOutDuration;
extern float PrimaryFadeInDuration;
extern float PrimaryFadeOutDuration;
extern float SecondaryFadeInDuration;
extern float SecondaryFadeOutDuration;
extern float Opacity;
extern float SecondaryItemX;
extern float ItemLoadY;
extern float ItemLoadQuickY;
extern float ItemClearListY;
extern float ItemCGLibraryY;
extern float ItemSoundLibraryY;
extern float ItemMovieLibraryY;
extern float ItemTipsY;
extern float ItemTrophyY;
extern float ItemConfigY;
extern float ItemSystemSaveY;
extern float SecondaryItemHighlightX;
extern float SecondaryMenuPaddingY;
extern float SecondaryMenuLoadOffsetY;
extern float SecondaryMenuLineX;
extern float SecondaryMenuLoadLineY;
extern float SecondaryMenuLoadQuickLineY;
extern float SecondaryMenuExtraClearY;
extern float SecondaryMenuExtraCGY;
extern float SecondaryMenuExtraSoundY;
extern float SecondaryMenuExtraMovieY;
extern float SecondaryMenuExtraTipsY;
extern float SecondaryMenuExtraTrophyY;
extern float SecondaryMenuSystemConfigY;
extern float SecondaryMenuSystemSaveY;

extern int LineNum;

}  // namespace TitleMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto