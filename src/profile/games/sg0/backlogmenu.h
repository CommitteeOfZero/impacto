#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace BacklogMenu {

void Configure();

inline glm::vec2 PanelPos;
inline float ShadeOpacity;     // dims the scene around the panel
inline RectF HighlightBounds;  // horizontal extent of the focus highlight
inline glm::vec3 HighlightColor;
inline float HighlightOpacity;

}  // namespace BacklogMenu
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
