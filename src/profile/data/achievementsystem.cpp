#include "achievementsystem.h"
#include "../profile_internal.h"

#include "../../games/chlcc/achievementsystem.h"

namespace Impacto {
namespace Profile {
namespace AchievementSystem {

using namespace Impacto::AchievementSystem;

void Configure() {
  EnsurePushMemberOfType("AchievementData", LUA_TTABLE);

  Type =
      AchievementDataType::_from_integral_unchecked(EnsureGetMemberInt("Type"));

  switch (Type) {
    case AchievementDataType::CHLCC:
      Implementation = new Impacto::CHLCC::AchievementSystem();
      break;
  }

  AchievementDataPath = EnsureGetMemberString("AchievementDataPath");
  
  Pop();
}
}  // namespace AchievementSystem
}  // namespace Profile
}  // namespace Impacto
