#include "scriptvars.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {
namespace ScriptVars {

// TODO: scope scriptvars for common and gamespecific
void Configure() {
  EnsurePushMemberOfType("ScriptVars", LUA_TTABLE);

  // clang-format off
  #define V(var, ...)                                              \
  __VA_OPT__(if (TryPushMember(#__VA_ARGS__)) {; AssertIs(LUA_TTABLE);) \
  __VA_OPT__(__VA_ARGS__::) var = EnsureGetMember<int>(#var);   \
  __VA_OPT__(Pop();})
  // clang-format on
#include "../scriptvars.h"
#undef V

  Pop();
}

}  // namespace ScriptVars
}  // namespace Profile
}  // namespace Impacto