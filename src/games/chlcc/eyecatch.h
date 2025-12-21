#pragma once

#include <cstdint>
#include "../../animation.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

struct EyecatchEffect {
  Sprite StarsMask;

  EyecatchEffect();
  void RenderMain();
  void RenderLayer(int layer);

  static EyecatchEffect& GetInstance() {
    static EyecatchEffect impl;
    return impl;
  };
};

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto