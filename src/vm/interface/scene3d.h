#pragma once

#include "../../animation.h"
#include <vector>
#include <ankerl/unordered_dense.h>

namespace Impacto {
namespace Vm {
namespace Interface {

void UpdateScene3D();

struct ScrWorkAnimationData {
  int From;
  int To;
  int Target;
};

struct ScrWorkAnimation {
  Animation MainAnimation;
  std::vector<ScrWorkAnimationData> AnimationData;
  bool AltTarget;
};

inline std::vector<int> CurrentScrWorkAnimations;
inline ankerl::unordered_dense::map<int, ScrWorkAnimation> ScrWorkAnimations;

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto