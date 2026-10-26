#pragma once

#include "../impacto.h"

namespace Impacto {
namespace Profile {
namespace ScriptVars {

// clang-format off
#define V(var, ...)                   \
  __VA_OPT__(namespace __VA_ARGS__ {) \
  inline int var;                     \
  __VA_OPT__(})
// clang-format on

inline int var;
#include "../scriptvars.h"
#undef V

void Configure();

}  // namespace ScriptVars
}  // namespace Profile
}  // namespace Impacto