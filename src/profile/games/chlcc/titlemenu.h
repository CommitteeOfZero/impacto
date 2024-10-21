#pragma once

#include "../../../spritesheet.h"
#include "../../../games/chlcc/titlemenu.h"
#include "../../../pathanimation.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TitleMenu {

void Configure();

inline int LineNum;
int constexpr LineEntriesNumMax = 32;
inline Sprite LineSprites[LineEntriesNumMax];

inline Sprite BackgroundSprite;

inline Sprite DelusionADVUnderSprite;
inline Sprite DelusionADVSprite;
inline float DelusionADVUnderX;
inline float DelusionADVUnderY;
inline float DelusionADVX;
inline float DelusionADVY;

inline Sprite SeiraUnderSprite;
inline Sprite SeiraSprite;
inline float SeiraUnderX;
inline float SeiraUnderY;
inline float SeiraX;
inline float SeiraY;

inline Sprite CHLogoSprite;
inline float CHLogoX;
inline float CHLogoY;

inline Sprite LCCLogoUnderSprite;
inline float LCCLogoUnderX;
inline float LCCLogoUnderY;

inline Sprite ChuLeftLogoSprite;
inline float ChuLeftLogoX;
inline float ChuLeftLogoY;

inline Sprite ChuRightLogoSprite;
inline float ChuRightLogoX;
inline float ChuRightLogoY;

inline Sprite LoveLogoSprite;
inline float LoveLogoX;
inline float LoveLogoY;

inline Sprite StarLogoSprite;
inline float StarLogoX;
inline float StarLogoY;

inline Sprite ExclMarkLogoSprite;
inline float ExclMarkLogoX;
inline float ExclMarkLogoY;

inline Sprite CopyrightTextSprite;
inline float CopyrightTextX;
inline float CopyrightTextY;

inline Sprite SpinningCircleSprite;
inline float SpinningCircleX;
inline float SpinningCircleY;
inline float SpinningCircleAnimationDuration;

inline Sprite ItemHighlightSprite;
inline Sprite ItemLoadHighlightedSprite;
inline Sprite SecondaryItemHighlightSprite;
inline float ItemHighlightOffsetX;
inline float ItemHighlightOffsetY;
inline float ItemPadding;
inline float ItemYBase;
inline float ItemFadeInDuration;
inline float ItemFadeOutDuration;

inline float PrimaryFadeInDuration;
inline float PrimaryFadeOutDuration;

inline float SecondaryFadeInDuration;
inline float SecondaryFadeOutDuration;
inline float SecondaryItemX;
inline float SecondaryItemFadeInDuration;
inline float SecondaryItemFadeOutDuration;

inline Sprite ItemLoadQuickSprite;
inline Sprite ItemLoadSprite;
inline Sprite ItemLoadQuickHighlightedSprite;
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

inline Sprite IntroBackgroundSprite;

constexpr size_t IntroHighlightCount = 10;
inline Sprite IntroHighlightSprites[IntroHighlightCount];
inline float IntroHighlightPositions[IntroHighlightCount];
inline float IntroPanningAnimationDuration;
inline float IntroAfterPanningWaitDuration;

inline Sprite IntroSmallStarSprite;
inline Sprite IntroBigStarSprite;

inline PathSegment* IntroStarBounceAnimationPath;
inline int IntroStarBounceAnimationSegmentCount;
inline Sprite IntroBouncingStarSprite;

inline float IntroExplodingStarAnimationDuration;
inline float IntroExplodingStarAnimationRotationDuration;
inline float IntroExplodingStarAnimationDistance;

}  // namespace TitleMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto