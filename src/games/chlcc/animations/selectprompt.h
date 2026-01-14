#pragma once

#include <vector>

#include "../../../animation.h"
#include "../../../spritesheet.h"
#include "../../../renderer/renderer.h"
#include "../../../profile/ui/gamespecific.h"

namespace Impacto {
namespace UI {
namespace CHLCC {
using namespace Impacto::Profile::GameSpecific;

class SelectPromptAnimation : public Animation {
 public:
  SelectPromptAnimation();
  void Draw(std::span<const Sprite> sprites,
            std::span<const glm::vec2> positions, glm::vec2 offset);
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto