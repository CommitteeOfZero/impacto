#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace BacklogMenu {

void Configure();

inline uint32_t BackgroundColor;
inline Sprite CircleSprite;
inline glm::vec2 MenuTitleTextRightPosition;
inline glm::vec2 MenuTitleTextLeftPosition;
inline float MenuTitleTextAngle;
inline Sprite MenuTitleText;

inline glm::vec2 ButtonPromptPosition;
inline Sprite ButtonPromptSprite;

}  // namespace BacklogMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto