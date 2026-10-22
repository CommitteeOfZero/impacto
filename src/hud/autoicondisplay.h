#pragma once

#include <glm/glm.hpp>

namespace Impacto {
namespace AutoIconDisplay {

enum class AutoIconType : int {
  None,
  SpriteAnim,
  SpriteAnimFixed,
  Fixed,
  CHLCC,
};
void Init();
void Update(float dt);
void Render(glm::vec4 opacityTint);

}  // namespace AutoIconDisplay
}  // namespace Impacto