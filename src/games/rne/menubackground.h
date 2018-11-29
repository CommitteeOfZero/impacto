#pragma once

#include "../../animation.h"

namespace Impacto {
namespace RNE {
class MenuBackground : public Animation {
 public:
  MenuBackground();
  void Render() override;
};
}  // namespace RNE
}  // namespace Impacto