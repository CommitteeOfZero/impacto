#pragma once

#include "../../spriteanimation.h"

namespace Impacto {
namespace Profile {
namespace LoadingDisplay {

inline SpriteAnimationDef ResourceLoadBgAnim;
inline SpriteAnimationDef SaveLoadBgAnim;
inline SpriteAnimationDef LoadingIconAnim;
inline SpriteAnimationDef LoadingTextAnim;

inline glm::vec2 ResourceBgPos;
inline glm::vec2 SaveBgPos;
inline glm::vec2 IconPos;
inline glm::vec2 TextPos;

inline float FadeInDuration;
inline float FadeOutDuration;

void Configure();

}  // namespace LoadingDisplay
}  // namespace Profile
}  // namespace Impacto