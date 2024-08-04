#pragma once

#include <enum.h>
#include <glm/glm.hpp>

#include "dialoguebox.h"

namespace Impacto {
namespace AutoIconDisplay {

BETTER_ENUM(AutoIconType, int, None, SpriteAnim, SpriteAnimFixed, Fixed, CHLCC)

void Init();
void Update(float dt);
void Render(glm::vec4 opacityTint);

}  // namespace AutoIconDisplay
}  // namespace Impacto