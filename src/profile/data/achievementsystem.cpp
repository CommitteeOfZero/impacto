#include "achievementsystem.h"
#include "../profile_internal.h"

#include "../../games/chlcc/achievementsystem.h"

namespace Impacto {
namespace Profile {
namespace AchievementSystem {

using namespace Impacto::AchievementSystem;

AchievementDataType Type = AchievementDataType::None;

std::string AchievementDataPath;

void Configure() {
  EnsurePushMemberOfType("AchievementData", kObjectType);

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
