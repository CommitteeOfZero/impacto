#pragma once

#include <glm/glm.hpp>

namespace Impacto {
namespace SaveIconDisplay {

void Init();
void Hide();
void Show();
void ShowAt(glm::vec2 pos);
void ShowFor(float seconds);
void Update(float dt);
void Render();

}  // namespace SaveIconDisplay
}  // namespace Impacto