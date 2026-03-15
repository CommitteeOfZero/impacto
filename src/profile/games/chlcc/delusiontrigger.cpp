#include "delusiontrigger.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../text/text.h"

#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/delusiontrigger.h"
namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace DelusionTrigger {

void Configure() {
  if (!TryPushMember("DelusionTrigger")) return;
  AssertIs(LUA_TTABLE);
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  BackgroundSpriteMask = EnsureGetMember<Sprite>("BackgroundSpriteMask");
  ScreenMask = EnsureGetMember<Sprite>("ScreenMask");

  BackgroundSpriteMask.Bounds.Y = BackgroundSprite.Bounds.Center().y -
                                  BackgroundSpriteMask.Bounds.Center().y;

  {
    EnsurePushMemberOfType("DelusionTextGlyphs", LUA_TTABLE);
    DelusionTextGlyphs = EnsureGet<std::array<std::vector<Sprite>, 21>>();

    Pop();
  }

  DelusionTextXVelocity = EnsureGetMember<float>("DelusionTextXVelocity");
  DelusionTextFadeDuration = EnsureGetMember<float>("DelusionTextFadeDuration");
  DelusionScaledGlyphWidth = EnsureGetMember<float>("DelusionScaledGlyphWidth");
  DelusionScaledGlyphHeight =
      EnsureGetMember<float>("DelusionScaledGlyphHeight");
  DelusionTextLineSpacing = EnsureGetMember<float>("DelusionTextLineSpacing");
  Pop();
}

}  // namespace DelusionTrigger
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto