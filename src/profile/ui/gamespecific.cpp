#include "gamespecific.h"
#include "../profile_internal.h"
#include "../../ui/ui.h"

#include "../games/chlcc/delusiontrigger.h"
#include "../games/cclcc/delusiontrigger.h"
#include "../games/cclcc/yesnotrigger.h"
#include "../games/cclcc/mapsystem.h"

namespace Impacto {
namespace Profile {
namespace GameSpecific {

using namespace Impacto::UI;

void Configure() {
  if (!TryPushMember("GameSpecific")) return;
  AssertIs(LUA_TTABLE);
  GameSpecificType =
      GameSpecificType::_from_integral_unchecked(EnsureGetMember<int>("Type"));

  // Get common gamespecific UI elements
  if (GameSpecificType == +GameSpecificType::CCLCC) {
    UIButtonGuideSprites = GetMemberVector<Sprite>("UIButtonGuideSprites");
    UIButtonGuideEndDisp = EnsureGetMember<RectF>("UIButtonGuideEndDisp");
  }
  if (GameSpecificType == +GameSpecificType::CHLCC) {
    MonitorScanline = EnsureGetMember<Sprite>("MonitorScanline");
    EyecatchStar = EnsureGetMember<Sprite>("EyecatchStar");
    ButterflySprites = GetMemberVector<Sprite>("ButterflySprites");
    ButterflyFrameCount = EnsureGetMember<uint8_t>("ButterflyFrameCount");
    ButterflyFlapFrameDuration =
        EnsureGetMember<float>("ButterflyFlapFrameDuration");
    ButterflyFadeDuration = EnsureGetMember<float>("ButterflyFadeDuration");

    BubbleSpriteSmall = EnsureGetMember<Sprite>("BubbleSpriteSmall");
    BubbleSpriteBig = EnsureGetMember<Sprite>("BubbleSpriteBig");
    BubbleFadeDuration = EnsureGetMember<float>("BubbleFadeDuration");
  }

  Pop();

  switch (GameSpecificType) {
    case +GameSpecificType::CHLCC:
      CHLCC::DelusionTrigger::Configure();
      break;
    case +GameSpecificType::CC:
      break;
    case +GameSpecificType::CCLCC:
      CCLCC::MapSystem::Configure();
      CCLCC::YesNoTrigger::Configure();
      CCLCC::DelusionTrigger::Configure();
      break;
    case +GameSpecificType::RNE:
      break;
    case +GameSpecificType::Dash:
      break;
    case +GameSpecificType::None:
      break;
  }
}

}  // namespace GameSpecific
}  // namespace Profile
}  // namespace Impacto