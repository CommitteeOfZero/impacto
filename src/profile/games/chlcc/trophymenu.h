#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TrophyMenu {

int constexpr inline MaxTrophyPages = 9;

inline float ShowPageAnimationStartTime;
inline float ShowPageAnimationDuration;

inline uint32_t BackgroundColor;
inline Sprite BackgroundFilter;

inline Sprite CircleSprite;
inline glm::vec2 CircleStartPosition;
inline float CircleOffset;

inline glm::vec2 ErinPosition;
inline Sprite ErinSprite;

inline glm::vec2 InitialRedBarPosition;
inline glm::vec2 RightRedBarPosition;
inline float RedBarDivision;
inline float RedBarBaseX;
inline Sprite InitialRedBarSprite;
inline glm::vec2 RedBarLabelPosition;
inline Sprite RedBarLabel;

inline Sprite RedBarSprite;
inline glm::vec2 RedBarPosition;

inline glm::vec2 MenuTitleTextRightPosition;
inline glm::vec2 MenuTitleTextLeftPosition;
inline float MenuTitleTextAngle;
inline float TitleFadeInDuration;
inline float TitleFadeOutDuration;
inline Sprite MenuTitleText;

inline glm::vec2 ButtonPromptPosition;
inline Sprite ButtonPromptSprite;

inline Sprite PlatinumTrophySprite;
inline glm::vec2 PlatinumTrophyPos;
inline Sprite GoldTrophySprite;
inline glm::vec2 GoldTrophyPos;
inline Sprite SilverTrophySprite;
inline glm::vec2 SilverTrophyPos;
inline Sprite BronzeTrophySprite;
inline glm::vec2 BronzeTrophyPos;

inline Sprite DefaultTrophyIconSprite;
inline Sprite TrophyEntryCardSprite;
inline Sprite TrophyEntriesBorderSprite;
inline Sprite TrophyPageCtBoxSprite;
inline glm::vec2 TrophyPageCtPos;

inline size_t EntriesPerPage;
inline glm::vec2 FirstEntryPos;
inline float EntryHeight;
inline glm::vec2 EntryCardOffset;
inline glm::vec2 EntryNameOffset;
inline float EntryNameFontSize;
inline glm::vec2 EntryDescriptionOffset;
inline float EntryDescriptionFontSize;
inline glm::vec2 EntryIconOffset;
inline uint32_t EntryDefaultNameTextTableId;
inline uint32_t EntryDefaultNameStringNum;

inline glm::vec2 CurrentPageNumPos;
inline Sprite PageNumSeparatorSlash;
inline glm::vec2 PageNumSeparatorPos;
inline glm::vec2 MaxPageNumPos;
inline std::array<Sprite, 10> PageNums;
inline std::array<Sprite, 10> ReachablePageNums;

inline uint32_t TrophyCountHintTextTableId;
inline uint32_t TrophyCountHintStringNum;
inline glm::vec2 TrophyCountHintLabelPos;
inline float TrophyCountFontSize;

void Configure();

}  // namespace TrophyMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto