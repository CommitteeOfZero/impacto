#pragma once

#include <enum.h>
#include <glm/glm.hpp>

namespace Impacto {
namespace WaitIconDisplay {

BETTER_ENUM(WaitIconType, int, None, SpriteAnim, RotateZ)

void Init();
void Update(float dt);
void Render(glm::vec2 pos, glm::vec4 opacityTint);

}  // namespace WaitIconDisplay
}  // namespace Impacto