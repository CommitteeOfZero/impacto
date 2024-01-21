#include "delusiontrigger.h"
#include "../profile_internal.h"
#include "../games/chlcc/delusiontrigger.h"
#include "../../games/chlcc/delusiontrigger.h"

namespace Impacto {
namespace Profile {
namespace DelusionTrigger {

using namespace Impacto::DelusionTrigger;

DelusionTriggerType Type = DelusionTriggerType::None;

float FadeInDuration;
float FadeOutDuration;

void Configure() {
  if (!TryPushMember("DelusionTrigger")) return;
  AssertIs(kObjectType);

  Type =
      DelusionTriggerType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

  switch (Type) {
    case DelusionTriggerType::CHLCC:
      CHLCC::DelusionTrigger::Configure();
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
      default:
        return;
    }
  }
}

}  // namespace DelusionTrigger
}  // namespace Profile
}  // namespace Impacto