#pragma once

#include <limits>
#include <vector>

#include "../spritesheet.h"

namespace Impacto {
namespace UI {

struct ButtonPromptIcon {
  uint32_t Button = 0;
  glm::vec2 Position = glm::vec2(0.0f);
};

struct ButtonPrompt {
  Sprite PromptSprite;
  std::vector<ButtonPromptIcon> Icons = {};
};

void DrawButtonPrompt(const ButtonPrompt& prompt, glm::vec2 pos,
                      glm::vec4 tint = glm::vec4(1.0f));
void DrawButtonPromptIcons(
    const ButtonPrompt& prompt, glm::vec2 pos, glm::vec4 tint = glm::vec4(1.0f),
    float maxWidth = std::numeric_limits<float>::infinity());

}  // namespace UI
}  // namespace Impacto
