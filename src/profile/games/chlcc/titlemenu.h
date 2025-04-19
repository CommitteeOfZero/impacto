#pragma once

#include "../../../spritesheet.h"
#include "../../../games/chlcc/titlemenu.h"

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
inline glm::vec2 DelusionADVPosition;
inline glm::vec2 DelusionADVPopoutOffset;

inline Sprite SeiraUnderSprite;
inline Sprite SeiraSprite;
inline glm::vec2 SeiraUnderPosition;
inline glm::vec2 SeiraPosition;
inline glm::vec2 SeiraPopoutOffset;

inline Sprite CHLogoSprite;
inline glm::vec2 CHLogoPosition;

inline Sprite LCCLogoUnderSprite;
inline glm::vec2 LCCLogoUnderPosition;

inline Sprite StarLogoSprite;
inline glm::vec2 StarLogoPosition;

inline Sprite CopyrightTextSprite;
inline glm::vec2 CopyrightTextPosition;

inline Sprite SpinningCircleSprite;
inline glm::vec2 SpinningCirclePosition;
inline float SpinningCircleAnimationDuration;

inline Sprite ItemHighlightSprite;
inline Sprite ItemLoadHighlightedSprite;
inline Sprite SecondaryItemHighlightSprite;
inline glm::vec2 ItemHighlightOffset;
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

constexpr size_t LCCLogoSpriteCount = 4;
inline std::array<glm::vec2, LCCLogoSpriteCount> LCCLogoPositions;
inline std::array<Sprite, LCCLogoSpriteCount> LCCLogoSprites;

inline Sprite IntroBackgroundSprite;

constexpr size_t IntroHighlightCount = 10;
inline std::array<Sprite, IntroHighlightCount> IntroHighlightSprites;
inline std::array<float, IntroHighlightCount> IntroHighlightPositions;

inline Sprite IntroExplodingStarSprite;
inline float IntroExplodingStarAnimationDistance;

inline Sprite IntroBouncingStarSprite;

inline Sprite IntroFallingStarSprite;
inline float IntroFallingStarsAnimationDistance;
inline glm::vec2 IntroFallingStarsAnimationDirection;

inline float IntroCHLogoFadeAnimationStartY;

inline Sprite IntroLogoStarHighlightSprite;
inline glm::vec2 IntroLogoStarHighlightPosition;

constexpr size_t IntroDelusionADVMaxSpriteCount = 7;
inline int IntroDelusionADVSpriteCount;
inline std::array<Sprite, IntroDelusionADVMaxSpriteCount>
    IntroDelusionADVSprites;
inline std::array<glm::vec2, IntroDelusionADVMaxSpriteCount>
    IntroDelusionADVPositions;

inline glm::vec2 IntroLogoPopOutOffset;

}  // namespace TitleMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto