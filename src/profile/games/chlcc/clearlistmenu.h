#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace ClearListMenu {

int const Endings = 8;

extern float MenuTransitionDuration;
extern uint32_t BackgroundColor;
extern Sprite CircleSprite;
extern glm::vec2 CircleStartPosition;
extern float CircleOffset;
extern glm::vec2 ErinPosition;
extern Sprite ErinSprite;
extern Sprite BackgroundFilter;
extern Sprite InitialRedBarSprite;
extern glm::vec2 RedBarPosition;
extern Sprite RedBarSprite;

extern glm::vec2 LabelPosition;
extern Sprite ClearListLabel;
extern glm::vec2 TimePositions[6];
extern glm::vec2 EndingCountPosition;
extern glm::vec2 TIPSCountPositions[2];
extern glm::vec2 AlbumPositions[3];
extern Sprite Digits[10];
extern glm::vec2 ListPosition;
extern Sprite EndingList;
extern glm::vec2 BoxPositions[Endings];
extern Sprite EndingBox;
extern glm::vec2 ThumbnailPositions[Endings];
extern Sprite EndingThumbnails[Endings];
extern Sprite LockedThumbnail;
extern glm::vec2 InitialRedBarPosition;
extern glm::vec2 RightRedBarPosition;

void Configure();

}  // namespace ClearListMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto