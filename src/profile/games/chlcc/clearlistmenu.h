#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace ClearListMenu{

int const Endings = 8;

extern uint32_t BackgroundColor;
extern glm::vec2 ErinPosition;
extern Sprite ErinSprite;
extern Sprite BackgroundFilter;

extern glm::vec2 LabelPosition;
extern Sprite ClearListLabel;
extern glm::vec2 TimePositions[6];
extern glm::vec2 EndingCountPosition;
extern glm::vec2 TIPSCountPositions[2];
extern Sprite Digits[10];
extern glm::vec2 ListPosition;
extern Sprite EndingList;
extern glm::vec2 BoxPositions[Endings];
extern Sprite EndingBox;
extern glm::vec2 ThumbnailPositions[Endings];
extern Sprite EndingThumbnails[Endings];
extern Sprite LockedThumbnail;

void Configure();

}  // namespace ClearListMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto