#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TrophyMenu {

int constexpr inline MaxTrophyPages = 9;

inline float MenuTransitionDuration;
inline uint32_t BackgroundColor;
inline Sprite CircleSprite;
inline glm::vec2 CircleStartPosition;
inline float CircleOffset;
inline glm::vec2 ErinPosition;
inline Sprite ErinSprite;
inline Sprite BackgroundFilter;
inline glm::vec2 InitialRedBarPosition;
inline Sprite InitialRedBarSprite;
inline glm::vec2 RightRedBarPosition;
inline glm::vec2 RedBarPosition;
inline float RedBarDivision;
inline float RedBarBaseX;
inline Sprite RedBarSprite;
inline glm::vec2 RedBarLabelPosition;
inline Sprite RedBarLabel;
inline glm::vec2 MenuTitleTextRightPosition;
inline glm::vec2 MenuTitleTextLeftPosition;
inline float MenuTitleTextAngle;
inline float TitleFadeInDuration;
inline float TitleFadeOutDuration;
inline Sprite MenuTitleText;
inline glm::vec2 ButtonPromptPosition;
inline Sprite ButtonPromptSprite;
inline Sprite DefaultTrophyIconSprite;
inline Sprite TrophyEntryCardSprite;
inline Sprite PlatinumTrophySprite;
inline glm::vec2 PlatinumTrophyPos;
inline Sprite GoldTrophySprite;
inline glm::vec2 GoldTrophyPos;
inline Sprite SilverTrophySprite;
inline glm::vec2 SilverTrophyPos;
inline Sprite BronzeTrophySprite;
inline glm::vec2 BronzeTrophyPos;
inline Sprite TrophyEntriesBorderSprite;
inline Sprite TrophyPageCtBoxSprite;
inline Sprite PageNums[10];
inline Sprite ReachablePageNums[10];
inline Sprite PageNumSeparatorSlash;
inline glm::vec2 CurrentPageNumPos;
inline glm::vec2 PageNumSeparatorPos;
inline glm::vec2 MaxPageNumPos;
inline glm::vec2 TrophyCountHintLabelPos;

void Configure();

}  // namespace TrophyMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto