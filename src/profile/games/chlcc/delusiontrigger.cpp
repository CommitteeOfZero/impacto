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

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  BackgroundSpriteMask = EnsureGetMember<Sprite>("BackgroundSpriteMask");
  ScreenMask = EnsureGetMember<Sprite>("ScreenMask");

  BackgroundSpriteMask.Bounds.Y = BackgroundSprite.Bounds.Center().y -
                                  BackgroundSpriteMask.Bounds.Center().y;
}

}  // namespace DelusionTrigger
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto