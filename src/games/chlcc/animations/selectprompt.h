#pragma once

#include "../../../animation.h"
#include "../../../spritesheet.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class SelectPromptAnimation : public Animation {
 public:
  SelectPromptAnimation();
  void Draw(std::span<const Sprite> sprites,
            std::span<const glm::vec2> positions, glm::vec2 offset);
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto