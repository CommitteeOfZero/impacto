#include "achievementsystem.h"
#include "../profile_internal.h"

#include "../../data/achievementsystemps3.h"

namespace Impacto {
namespace Profile {
namespace AchievementSystem {

using namespace Impacto::AchievementSystem;

void Configure() {
  if (TryPushMember("AchievementData")) {
    AssertIs(LUA_TTABLE);
    Type = AchievementDataType::_from_integral_unchecked(
        EnsureGetMemberInt("Type"));

    switch (Type) {
      case AchievementDataType::CHLCC:
        Implementation = new Impacto::AchievementSystem::AchievementSystemPS3();
        break;
    }

    AchievementDataPath = EnsureGetMemberString("AchievementDataPath");

    Pop();
  }
}
}  // namespace AchievementSystem
}  // namespace Profile
}  // namespace Impacto
