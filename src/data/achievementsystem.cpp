#include "achievementsystem.h"
#include "../profile/data/achievementsystem.h"

namespace Impacto {
namespace AchievementSystem {

using namespace Impacto::Profile::AchievementSystem;

void Init() { Configure(); }

bool MountAchievementFile() {
  if (Implementation)
    return Implementation->MountAchievementFile();
  else
    return false;
}

const Achievement* GetAchievement(int id) {
  if (Implementation)
    return Implementation->GetAchievement(id);
  else
    return nullptr;
}

}  // namespace AchievementSystem
}  // namespace Impacto