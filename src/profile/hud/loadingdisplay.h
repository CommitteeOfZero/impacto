#pragma once

#include "../../spriteanimation.h"

namespace Impacto {
namespace Profile {
namespace LoadingDisplay {

extern SpriteAnimationDef ResourceLoadBgAnim;
extern SpriteAnimationDef SaveLoadBgAnim;
extern SpriteAnimationDef LoadingIconAnim;
extern SpriteAnimationDef LoadingTextAnim;

extern glm::vec2 ResourceBgPos;
extern glm::vec2 SaveBgPos;
extern glm::vec2 IconPos;
extern glm::vec2 TextPos;

extern float FadeInDuration;
extern float FadeOutDuration;

void Configure();

}  // namespace LoadingDisplay
}  // namespace Profile
}  // namespace Impacto