#include "delusiontrigger.h"
#include "../../profile_internal.h"
#include "../../../log.h"

#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/chlcc/delusiontrigger.h"
#include "../../game.h"
#include "../../patch.h"

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

  if (Patch::HasDelusionMouseSupport) {
    LeftDelusionHeartSprite =
        EnsureGetMember<Sprite>("LeftDelusionHeartSprite");
    RightDelusionHeartSprite =
        EnsureGetMember<Sprite>("RightDelusionHeartSprite");
    LeftDelusionHeartPos = EnsureGetMember<glm::vec2>("LeftDelusionHeartPos");
    RightDelusionHeartPos = EnsureGetMember<glm::vec2>("RightDelusionHeartPos");
    DelusionHeartPulseDuration =
        EnsureGetMember<float>("DelusionHeartPulseDuration");
    DelusionHeartPulseScale = EnsureGetMember<float>("DelusionHeartPulseScale");
  }

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
