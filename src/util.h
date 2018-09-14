#pragma once

#include "impacto.h"
#include <glm/glm.hpp>

// TODO own _malloca for gcc

#if defined(_malloca)
#define ImpStackAlloc _malloca
#define ImpStackFree _freea
#else
#define ImpStackAlloc malloc
#define ImpStackFree free
#endif

namespace Impacto {
char* DumpMat4(glm::mat4* matrix, const char* columnSeparator = "\t",
               const char* rowSeparator = "\n");

// Thanks https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
inline int Uint32Log2(uint32_t v) {
  unsigned int const b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
  unsigned int const S[] = {1, 2, 4, 8, 16};
  int i;

  register unsigned int r = 0;  // result of log2(v) will go here
  for (i = 4; i >= 0; i--)      // unroll for speed...
  {
    if (v & b[i]) {
      v >>= S[i];
      r |= S[i];
    }
  }
  return r;
}
}  // namespace Impacto