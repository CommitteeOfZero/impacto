#include "achievementsystem.h"
#include "../profile/data/achievementsystem.h"

namespace Impacto {
namespace AchievementSystem {

using namespace Impacto::Profile::AchievementSystem;

AchievementSystemBase *Implementation = nullptr;

void Init() { Configure(); }

AchievementError MountAchievementFile() {
  if (Implementation)
    return Implementation->MountAchievementFile();
  else
    return AchievementOK;
}

}  // namespace AchievementSystem
}  // namespace Impacto