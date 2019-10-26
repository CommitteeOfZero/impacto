#pragma once

#include "../impacto.h"

namespace Impacto {
namespace Profile {
namespace ScriptVars {

#define V(var) extern int var;
#include "../scriptvars.h"
#undef V

void Configure();

}  // namespace ScriptVars
}  // namespace Profile
}  // namespace Impacto