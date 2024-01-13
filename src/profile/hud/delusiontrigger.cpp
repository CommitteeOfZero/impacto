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
Sprite BackgroundSprite;
Sprite BackgroundSpriteMask;
Sprite ScreenMask;

void Configure() {
  EnsurePushMemberOfType("DelusionTrigger", kObjectType);

  Type = DelusionTriggerType::_from_integral_unchecked(
      EnsureGetMemberInt("Type"));

  switch (Type) {
    case DelusionTriggerType::CHLCC:
      CHLCC::DelusionTrigger::Configure();
      break;
    default:
      Pop();
      return;
  }
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  BackgroundSpriteMask = EnsureGetMemberSprite("BackgroundSpriteMask");
  ScreenMask = EnsureGetMemberSprite("ScreenMask");
  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

  BackgroundSpriteMask.Bounds.Y = BackgroundSprite.Bounds.Center().y - BackgroundSpriteMask.Bounds.Center().y;

  Pop();
}
void CreateInstance() {
  if (!Impacto::DelusionTrigger::Implementation) {
    switch (Type) {
      case DelusionTriggerType::CHLCC:
        Impacto::DelusionTrigger::Implementation = new Impacto::CHLCC::DelusionTrigger;
        break;
      default:
        return;
    }
  }
}

}  // namespace TipsNotification
}  // namespace Profile
}  // namespace Impacto