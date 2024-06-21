#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TipsMenu {

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
inline glm::vec2 TreePosition;
inline Sprite TipsTree;
inline glm::vec2 GradientPosition;
inline Sprite TipsGradient;
inline uint32_t EndOfGradientColor;
inline glm::vec2 CurrentTipBackgroundPosition;
inline Sprite CurrentTipBackgroundSprite;
inline glm::vec2 ButtonPromptPosition;
inline Sprite ButtonPromptSprite;

void Configure();

}  // namespace TipsMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto