#pragma once

#include "../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

class EyecatchEffect {
 public:
  MutableTextureRef StarsMask;

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