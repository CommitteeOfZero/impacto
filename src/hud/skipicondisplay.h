#pragma once

#include <enum.h>
#include <glm/glm.hpp>

#include "dialoguebox.h"

namespace Impacto {
namespace SkipIconDisplay {

BETTER_ENUM(SkipIconType, int, None, SpriteAnim, SpriteAnimFixed, Fixed)

void Init();
void Update(float dt);
void Render(glm::vec4 opacityTint);

} // namespace SkipIconDisplay
} // namespace Impacto