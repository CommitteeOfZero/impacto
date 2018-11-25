#pragma once

#include <glm/glm.hpp>

namespace Impacto {
namespace SaveIconDisplay {

void Hide();
void Show();
void ShowAt(glm::vec2 pos);
void Update(float dt);
void Render();

}  // namespace SaveIconDisplay
}  // namespace Impacto