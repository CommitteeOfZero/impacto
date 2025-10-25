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
  }
  Pop();

  // Configure game-specific subsystems
  if (GameSpecificType == +GameSpecificType::CHLCC) {
    Profile::CHLCC::DelusionTrigger::Configure();
  } else if (GameSpecificType == +GameSpecificType::CC) {
  } else if (GameSpecificType == +GameSpecificType::CCLCC) {
    Profile::CCLCC::MapSystem::Configure();
    Profile::CCLCC::YesNoTrigger::Configure();
    Profile::CCLCC::DelusionTrigger::Configure();
  }
}

}  // namespace GameSpecific
}  // namespace Profile
}  // namespace Impacto