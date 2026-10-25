#pragma once

#include "../../../spritesheet.h"
#include "../../../ui/buttonprompt.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace BacklogMenu {

void Configure();

inline Sprite BacklogHeaderSprite;
inline glm::vec2 BacklogHeaderPosition;

inline UI::ButtonPrompt BacklogControlsSprite;
inline glm::vec2 BacklogControlsPosition;

inline SpriteSheet BacklogMaskSheet;

inline float FadeInDirectDuration;
inline float FadeOutDirectDuration;

}  // namespace BacklogMenu
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto