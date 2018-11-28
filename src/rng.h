#pragma once

#include <pcg_basic.h>
#include <math.h>

namespace Impacto {

class RNG {
 public:
  RNG() { Seed(0, 0); }
  RNG(uint64_t Seed1, uint64_t Seed2) { Seed(Seed1, Seed2); }
  void Seed(uint64_t Seed1, uint64_t Seed2) {
    pcg32_srandom_r(&Pcg, Seed1, Seed2);
  }

  uint32_t UintBetween(uint32_t min, uint32_t maxExclusive) {
    return pcg32_boundedrand_r(&Pcg, (maxExclusive - min)) + min;
  }

  float FloatBetween(float min, float maxExclusive) {
    return (maxExclusive - min) * ldexpf(pcg32_random_r(&Pcg), -32) + min;
  }

 private:
  pcg32_random_t Pcg;
};

}  // namespace Impacto