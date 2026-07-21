#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace BacklogMenu {

void Configure();

inline Sprite BacklogHeaderSprite;
inline glm::vec2 BacklogHeaderPosition;

inline Sprite BacklogControlsSprite;
inline glm::vec2 BacklogControlsPosition;

inline SpriteSheet BacklogMaskSheet;

inline float FadeInDirectDuration;
inline float FadeOutDirectDuration;

}  // namespace BacklogMenu
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto