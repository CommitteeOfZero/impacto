#pragma once

#include <magic_enum/magic_enum.hpp>
#include <glm/glm.hpp>

#include "dialoguebox.h"

namespace Impacto {
namespace SkipIconDisplay {

enum class SkipIconType : int {
  None,
  SpriteAnim,
  SpriteAnimFixed,
  Fixed,
  CHLCC,
};
void Init();
void Update(float dt);
void Render(glm::vec4 opacityTint);

}  // namespace SkipIconDisplay
}  // namespace Impacto