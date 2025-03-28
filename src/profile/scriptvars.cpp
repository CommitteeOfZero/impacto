#include "scriptvars.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {
namespace ScriptVars {

// TODO: scope scriptvars for common and gamespecific
void Configure() {
  EnsurePushMemberOfType("ScriptVars", LUA_TTABLE);

#define V(var) var = TryGetMember<int>(#var).value_or(-1);
#include "../scriptvars.h"
#undef V

  Pop();
}

}  // namespace ScriptVars
}  // namespace Profile
}  // namespace Impacto