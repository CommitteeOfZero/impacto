#pragma once

#include <enum.h>
#include <glm/glm.hpp>

#include "dialoguebox.h"

namespace Impacto {
namespace WaitIconDisplay {

BETTER_ENUM(WaitIconType, int, None, SpriteAnim, SpriteAnimFixed, Rotate, Fixed,
            RotateZ)

void Init();
void Update(float dt);
void Render(glm::vec2 pos, glm::vec4 opacityTint, DialoguePageMode mode);

}  // namespace WaitIconDisplay
}  // namespace Impacto