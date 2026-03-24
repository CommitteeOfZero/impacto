#pragma once

#include <magic_enum/magic_enum.hpp>
#include <glm/glm.hpp>

#include "dialoguebox.h"

namespace Impacto {
namespace WaitIconDisplay {

enum class WaitIconType : int {
  None,
  SpriteAnim,
  SpriteAnimFixed,
  Rotate,
  Fixed,
  RotateZ,
};
void Init();
void Update(float dt);
void Render(glm::vec2 pos, glm::vec4 opacityTint, DialoguePageMode mode,
            int dialoguePageId);

}  // namespace WaitIconDisplay
}  // namespace Impacto