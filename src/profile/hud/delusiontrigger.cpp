#include "delusiontrigger.h"
#include "../profile_internal.h"
#include "../games/chlcc/delusiontrigger.h"
#include "../games/cclcc/delusiontrigger.h"
#include "../../games/chlcc/delusiontrigger.h"
#include "../../games/cclcc/delusiontrigger.h"

namespace Impacto {
namespace Profile {
namespace DelusionTrigger {

using namespace Impacto::DelusionTrigger;

void Configure() {
  if (!TryPushMember("DelusionTrigger")) return;
  AssertIs(LUA_TTABLE);

  Type = DelusionTriggerType::_from_integral_unchecked(
      EnsureGetMember<int>("Type"));

  switch (Type) {
    case DelusionTriggerType::CHLCC:
      CHLCC::DelusionTrigger::Configure();
      break;
    case DelusionTriggerType::CCLCC:
      CCLCC::DelusionTrigger::Configure();
      break;
    default:
      Pop();
      return;
  }

  Pop();
}
void CreateInstance() {
  if (!Impacto::DelusionTrigger::Implementation) {
    switch (Type) {
      case DelusionTriggerType::CHLCC:
        Impacto::DelusionTrigger::Implementation =
            new Impacto::CHLCC::DelusionTrigger;
        break;
      case DelusionTriggerType::CCLCC:
        Impacto::DelusionTrigger::Implementation =
            new Impacto::CCLCC::DelusionTrigger;
        break;
      default:
        return;
    }
  }
}

}  // namespace DelusionTrigger
}  // namespace Profile
}  // namespace Impacto