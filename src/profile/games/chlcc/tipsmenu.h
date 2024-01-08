#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TipsMenu {

extern float MenuTransitionDuration;
extern uint32_t BackgroundColor;
extern Sprite CircleSprite;
extern glm::vec2 CircleStartPosition;
extern float CircleOffset;
extern glm::vec2 ErinPosition;
extern Sprite ErinSprite;
extern Sprite BackgroundFilter;
extern glm::vec2 InitialRedBarPosition;
extern Sprite InitialRedBarSprite;
extern glm::vec2 RightRedBarPosition;
extern glm::vec2 RedBarPosition;
extern float RedBarDivision;
extern float RedBarBaseX;
extern Sprite RedBarSprite;
extern glm::vec2 RedBarLabelPosition;
extern Sprite RedBarLabel;
extern glm::vec2 MenuTitleTextRightPosition;
extern glm::vec2 MenuTitleTextLeftPosition;
extern float MenuTitleTextAngle;
extern float TitleFadeInDuration;
extern float TitleFadeOutDuration;
extern Sprite MenuTitleText;
extern glm::vec2 TreePosition;
extern Sprite TipsTree;
extern glm::vec2 GradientPosition;
extern Sprite TipsGradient;
extern uint32_t EndOfGradientColor;
extern glm::vec2 CurrentTipBackgroundPosition;
extern Sprite CurrentTipBackgroundSprite;
extern glm::vec2 ButtonPromptPosition;
extern Sprite ButtonPromptSprite;

void Configure();

}  // namespace TipsMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto