#pragma once

#include <vector>

#include "../../../animation.h"
#include "../../../spritesheet.h"
#include "../../../renderer/renderer.h"
#include "../../../profile/ui/gamespecific.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class MenuTransitionAnimation : public Animation {
 public:
  MenuTransitionAnimation();
  glm::vec2 GetPageOffset() const;
};
}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto