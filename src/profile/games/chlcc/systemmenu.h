#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SystemMenu {
constexpr int MenuEntriesNumMax = 8;

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

inline Sprite MenuBackground;
inline glm::vec2 MenuBackgroundPosition;
inline Sprite MenuItemsLine;
inline glm::vec2 MenuItemsLinePosition;
inline Sprite MainMenuLabel;
inline glm::vec2 MainMenuLabelPosition;
inline glm::vec2 MainMenuLabelRightPosition;
inline float MainMenuLabelRightAngle;
inline Sprite MenuSelectionDot;
inline glm::vec2 MenuSelectionDotPosition;
inline float MenuSelectionDotMultiplier;
inline Sprite MenuSelection;
inline glm::vec2 MenuSelectionPosition;
inline Sprite SelectMenuHeader;
inline glm::vec2 SelectMenuHeaderPosition;
inline Sprite MenuRunningSelectedLabel;
inline Sprite MenuButtonPrompt;
inline glm::vec2 MenuButtonPromptPosition;
inline Sprite MenuLine;
inline glm::vec2 MenuLinePosition;
inline Sprite MenuCHLCCLabel;
inline glm::vec2 MenuCHLCCLabelPosition;


void Configure();

}  // namespace SystemMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto