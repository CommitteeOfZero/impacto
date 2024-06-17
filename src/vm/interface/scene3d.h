#pragma once

#include "../../animation.h"
#include <vector>
#include <flat_hash_map.hpp>

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
inline ska::flat_hash_map<int, ScrWorkAnimation> ScrWorkAnimations;

}  // namespace Interface
}  // namespace Vm
}  // namespace Impacto