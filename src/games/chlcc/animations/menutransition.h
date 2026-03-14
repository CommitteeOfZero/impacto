#pragma once

#include "../../../animation.h"
#include "../../../renderer/renderer.h"

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