#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace TitleMenu {

void Configure();

int constexpr SubItemsNum = 7;

inline Sprite LogoSprite;
inline glm::vec2 LogoPos;

inline glm::vec2 ItemPos;
inline float ItemSpacing;
inline Sprite SubItemSprites[SubItemsNum];
inline Sprite SubItemHSprites[SubItemsNum];
inline float SubItemOffsetX;
inline float SubItemSpacing;

inline float PressToStartAnimFastDurationIn;
inline float PressToStartAnimFastDurationOut;

}  // namespace TitleMenu
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
