#pragma once

#include "../../spriteanimation.h"

namespace Impacto {
namespace Profile {
namespace SaveIcon {

inline SpriteAnimationDef ForegroundAnimation;
inline glm::vec2 DefaultPosition;
inline Sprite BackgroundSprite;
inline glm::vec2 BackgroundOffset;
inline float BackgroundMaxAlpha;
inline float FadeInDuration;
inline float FadeOutDuration;

void Configure();

}  // namespace SaveIcon
}  // namespace Profile
}  // namespace Impacto