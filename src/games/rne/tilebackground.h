#pragma once

#include "../../animation.h"

namespace Impacto {
namespace RNE {
class TileBackground : public Animation {
 public:
  TileBackground();
  void Render() override;
};
}  // namespace RNE
}  // namespace Impacto