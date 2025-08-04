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
        EnsureGetMember<int>("Type"));

    switch (Type) {
      case AchievementDataType::PS3:
        Implementation = new Impacto::AchievementSystem::AchievementSystemPS3();
        break;

      case AchievementDataType::None:
        break;
    }

    AchievementDataPath = EnsureGetMember<std::string>("AchievementDataPath");

    Pop();
  }
}
}  // namespace AchievementSystem
}  // namespace Profile
}  // namespace Impacto
