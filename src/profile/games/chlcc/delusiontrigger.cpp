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
    ForEachProfileArray([](uint32_t line) {
      AssertIs(LUA_TTABLE);
      ForEachProfileArray([line](uint32_t) {
        DelusionTextGlyphs[line].emplace_back(EnsureGetArrayElement<Sprite>());
      });
    });

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