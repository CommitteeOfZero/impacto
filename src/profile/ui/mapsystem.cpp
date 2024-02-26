#include "mapsystem.h"
#include "../profile_internal.h"

#include "../games/cclcc/mapsystem.h"
#include "../../games/cclcc/mapsystem.h"
#include "../../profile/games/cclcc/mapsystem.h"

namespace Impacto {
namespace Profile {
namespace MapSystem {

void Configure() {
  if (!TryPushMember("MapSystem")) return;
  AssertIs(LUA_TTABLE);
  CCLCC::MapSystem::Configure();
  Pop();
}
void CreateInstance() {
  if (!Impacto::UI::MapSystem::MapSystemPtr) {
    Impacto::UI::MapSystem::MapSystemPtr =
        new Impacto::UI::CCLCC::MapSystemCCLCC;
  }
}
}  // namespace MapSystem
}  // namespace Profile
}  // namespace Impacto