#include "scriptvars.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {
namespace ScriptVars {

void Configure() {
  EnsurePushMemberOfType("ScriptVars", LUA_TTABLE);

#define V(var) var = EnsureGetMember<int>(#var);
#include "../scriptvars.h"
#undef V

  Pop();
}

}  // namespace ScriptVars
}  // namespace Profile
}  // namespace Impacto