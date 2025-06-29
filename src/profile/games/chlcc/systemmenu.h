#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SystemMenu {
constexpr int MenuEntriesNumMax = 9;
constexpr int SelectMenuTextMax = 10;

inline uint32_t BackgroundColor;
inline Sprite BackgroundFilter;
inline glm::vec2 CircleStartPosition;
inline Sprite CircleSprite;
inline float CircleOffset;
inline glm::vec2 ErinPosition;
inline Sprite ErinSprite;
inline float TitleFadeInDuration;
inline float TitleFadeOutDuration;
inline uint32_t FocusTint;
inline glm::vec2 MenuEntriesPositions[MenuEntriesNumMax];
inline Sprite Background;
inline glm::vec2 BackgroundPosition;

inline float MenuLoopDuration;
inline Sprite MenuBackground;
inline glm::vec2 MenuBackgroundPosition;
inline Sprite MenuItemsLine;
inline glm::vec2 MenuItemsLinePosition;
inline Sprite MainMenuTitleText;
inline glm::vec2 MenuTitleTextPosition;
inline glm::vec2 MainMenuLabelRightPosition;
inline float MenuTitleTextAngle;
inline Sprite MenuSelectionDot;
inline glm::vec2 MenuSelectionDotPosition;
inline float MenuSelectionDotMultiplier;
inline Sprite MenuSelection;
inline glm::vec2 MenuSelectionPosition;
inline Sprite SelectMenuHeader[SelectMenuTextMax];
inline glm::vec2 SelectMenuHeaderPositions[SelectMenuTextMax];
inline int SelectMenuHeaderCount;
inline Sprite MenuRunningSelectedLabel;
inline glm::vec2 MenuRunningSelectedLabelPosition;
inline float MenuRunningSelectedLabelAngle;
inline Sprite MenuButtonPrompt;
inline glm::vec2 MenuButtonPromptPosition;
inline glm::vec2 InitialRedBarPosition;
inline Sprite RedBarSprite;
inline Sprite InitialRedBarSprite;
inline glm::vec2 RightRedBarPosition;
inline glm::vec2 RedBarPosition;
inline float RedBarDivision;
inline float RedBarBaseX;
inline glm::vec2 MenuTitleTextRightPosition;
inline glm::vec2 RedBarLabelPosition;
inline Sprite RedBarLabel;
inline glm::vec2 MenuCHLCCLabelPosition;
inline float SelectedLabelSpeed;
inline float HoverLerpSpeed;

void Configure();

}  // namespace SystemMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto