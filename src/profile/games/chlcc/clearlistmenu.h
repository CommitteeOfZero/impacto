#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace ClearListMenu {

int constexpr Endings = 8;

inline uint32_t BackgroundColor;
inline Sprite CircleSprite;
inline glm::vec2 MenuTitleTextRightPosition;
inline glm::vec2 MenuTitleTextLeftPosition;
inline float MenuTitleTextAngle;
inline Sprite MenuTitleText;

inline glm::vec2 LabelPosition;
inline Sprite ClearListLabel;
inline glm::vec2 TimePositions[6];
inline glm::vec2 EndingCountPosition;
inline glm::vec2 TIPSCountPositions[3];
inline glm::vec2 AlbumPositions[3];
inline Sprite Digits[10];
inline glm::vec2 ListPosition;
inline Sprite EndingList;
inline glm::vec2 BoxPositions[Endings];
inline Sprite EndingBox;
inline glm::vec2 ThumbnailPositions[Endings];
inline Sprite EndingThumbnails[Endings];
inline Sprite LockedThumbnail;
inline glm::vec2 ButtonPromptPosition;
inline Sprite ButtonPromptSprite;

void Configure();

}  // namespace ClearListMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto