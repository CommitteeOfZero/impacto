#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace SysMesBox {

void Configure();

inline Sprite BackgroundSprite;
inline float BackgroundOpacity;

inline uint32_t MessageColor;
inline uint32_t ChoiceColor;
inline uint32_t ChoiceHighlightColor;
inline uint32_t OutlineColor;
inline float ChoiceSpacing;
inline glm::vec2 ChoicePadding;

}  // namespace SysMesBox
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
