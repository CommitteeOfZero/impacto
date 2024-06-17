#pragma once

#include "../../../spritesheet.h"
#include "../../../games/chlcc/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TitleMenu {

void Configure();

int constexpr LineEntriesNumMax = 32;

inline Sprite IntroBackgroundSprite;
inline Sprite BackgroundSprite;
inline Sprite DelusionADVUnderSprite;
inline Sprite DelusionADVSprite;
inline Sprite SeiraUnderSprite;
inline Sprite SeiraSprite;
inline Sprite CHLogoSprite;
inline Sprite LCCLogoUnderSprite;
inline Sprite ChuLeftLogoSprite;
inline Sprite ChuRightLogoSprite;
inline Sprite LoveLogoSprite;
inline Sprite StarLogoSprite;
inline Sprite ExclMarkLogoSprite;
inline Sprite CopyrightTextSprite;
inline Sprite SpinningCircleSprite;
inline Sprite ItemHighlightSprite;
inline Sprite LineSprites[LineEntriesNumMax];
inline Sprite ItemLoadQuickSprite;
inline Sprite ItemLoadSprite;
inline Sprite ItemLoadQuickHighlightedSprite;
inline Sprite ItemLoadHighlightedSprite;
inline Sprite SecondaryItemHighlightSprite;

inline float DelusionADVUnderX;
inline float DelusionADVUnderY;
inline float DelusionADVX;
inline float DelusionADVY;
inline float SeiraUnderX;
inline float SeiraUnderY;
inline float SeiraX;
inline float SeiraY;
inline float CHLogoX;
inline float CHLogoY;
inline float LCCLogoUnderX;
inline float LCCLogoUnderY;
inline float ChuLeftLogoX;
inline float ChuLeftLogoY;
inline float ChuRightLogoX;
inline float ChuRightLogoY;
inline float LoveLogoX;
inline float LoveLogoY;
inline float StarLogoX;
inline float StarLogoY;
inline float ExclMarkLogoX;
inline float ExclMarkLogoY;
inline float CopyrightTextX;
inline float CopyrightTextY;
inline float SpinningCircleX;
inline float SpinningCircleY;
inline float SpinningCircleAnimationDuration;
inline float ItemHighlightOffsetX;
inline float ItemHighlightOffsetY;
inline float ItemPadding;
inline float ItemYBase;
inline float ItemFadeInDuration;
inline float ItemFadeOutDuration;
inline float SecondaryItemFadeInDuration;
inline float SecondaryItemFadeOutDuration;
inline float PrimaryFadeInDuration;
inline float PrimaryFadeOutDuration;
inline float SecondaryFadeInDuration;
inline float SecondaryFadeOutDuration;
inline float SecondaryItemX;
inline float ItemLoadY;
inline float ItemLoadQuickY;
inline float ItemClearListY;
inline float ItemCGLibraryY;
inline float ItemSoundLibraryY;
inline float ItemMovieLibraryY;
inline float ItemTipsY;
inline float ItemTrophyY;
inline float ItemConfigY;
inline float ItemSystemSaveY;
inline float SecondaryItemHighlightX;
inline float SecondaryMenuPaddingY;
inline float SecondaryMenuLoadOffsetY;
inline float SecondaryMenuLineX;
inline float SecondaryMenuLoadLineY;
inline float SecondaryMenuLoadQuickLineY;
inline float SecondaryMenuExtraClearY;
inline float SecondaryMenuExtraCGY;
inline float SecondaryMenuExtraSoundY;
inline float SecondaryMenuExtraMovieY;
inline float SecondaryMenuExtraTipsY;
inline float SecondaryMenuExtraTrophyY;
inline float SecondaryMenuSystemConfigY;
inline float SecondaryMenuSystemSaveY;

inline int LineNum;

}  // namespace TitleMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto