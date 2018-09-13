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
}