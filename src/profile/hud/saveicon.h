#pragma once

#include "../../spriteanimation.h"

namespace Impacto {
namespace Profile {
namespace SaveIcon {

BETTER_ENUM(SaveIconType, int, Default, CHLCC)
inline SaveIconType SaveIconCurrentType = SaveIconType::Default;

inline SpriteAnimationDef ForegroundAnimation;
inline glm::vec2 DefaultPosition;
inline Sprite BackgroundSprite;
inline glm::vec2 BackgroundOffset;
inline float BackgroundMaxAlpha;
inline float FadeInDuration;
inline float FadeOutDuration;

inline std::vector<Sprite> SaveIconSprites;
inline float ActiveAnimationDuration;

void Configure();

}  // namespace SaveIcon
}  // namespace Profile
}  // namespace Impacto