#include "delusiontrigger.h"
#include "../../profile_internal.h"

#include "../../../ui/ui.h"
#include "../../../games/cclcc/delusiontrigger.h"
namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace DelusionTrigger {

void Configure() {
  if (!TryPushMember("DelusionTrigger")) return;
  AssertIs(LUA_TTABLE);
  DragDelta = EnsureGetMember<float>("DragDelta");
  Pop();
}

}  // namespace DelusionTrigger
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto
