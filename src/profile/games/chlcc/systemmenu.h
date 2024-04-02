#pragma once

#include "../../../spritesheet.h"

int const MenuEntriesNum = 8;

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SystemMenu {

extern uint32_t BackgroundColor;
extern Sprite BackgroundFilter;
extern glm::vec2 CircleStartPosition;
extern Sprite CircleSprite;
extern float CircleOffset;
extern glm::vec2 ErinPosition;
extern Sprite ErinSprite;
extern float TitleFadeInDuration;
extern float TitleFadeOutDuration;
extern uint32_t FocusTint;
extern glm::vec2 MenuEntriesPositions[MenuEntriesNum];

void Configure();

}  // namespace SystemMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto