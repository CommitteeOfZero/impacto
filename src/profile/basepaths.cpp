#include "basepaths.h"

#include "profile_internal.h"
#include "../io/filemeta.h"
namespace Impacto::Profile::BasePaths {

void Configure() {
  EnsurePushMemberOfType("BasePaths", LUA_TTABLE);
  RootGamedataDir = EnsureGetMember<std::string>("RootGamedataDir");
  RootProfilesDir = EnsureGetMember<std::string>("RootProfilesDir");
  RootPatchesDir = TryGetMember<std::string>("RootPatchesDir").value_or("");
  RootSavesDir = EnsureGetMember<std::string>("RootSavesDir");
  Pop();
}
}  // namespace Impacto::Profile::BasePaths