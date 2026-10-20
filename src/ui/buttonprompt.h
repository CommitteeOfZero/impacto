#pragma once

#include <limits>
#include <vector>
#include <variant>

#include "../spritesheet.h"
#include "../vm/interface/input.h"

namespace Impacto {
namespace UI {

struct ButtonPromptIcon {
  using KeyType = std::variant<std::string, Vm::Interface::PADinput>;
  KeyType Key{};
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
