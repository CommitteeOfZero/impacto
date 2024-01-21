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

Sprite BackgroundSprite;
Sprite BackgroundSpriteMask;
Sprite ScreenMask;

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  BackgroundSpriteMask = EnsureGetMemberSprite("BackgroundSpriteMask");
  ScreenMask = EnsureGetMemberSprite("ScreenMask");

  BackgroundSpriteMask.Bounds.Y = BackgroundSprite.Bounds.Center().y -
                                  BackgroundSpriteMask.Bounds.Center().y;
}

}  // namespace DelusionTrigger
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto