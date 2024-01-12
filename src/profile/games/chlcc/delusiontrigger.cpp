#include "delusiontrigger.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../text.h"

#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/delusiontrigger.h"
namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace DelusionTrigger {


float TextFadeInDuration;
float TextFadeOutDuration;

void Configure() {
  TextFadeInDuration = EnsureGetMemberFloat("TextFadeInDuration");
  TextFadeOutDuration = EnsureGetMemberFloat("TextFadeOutDuration");
}

}  // namespace DelusionTrigger
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto