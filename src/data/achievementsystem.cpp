#include "achievementsystem.h"
#include "../mem.h"
#include "../loadable.h"
#include "../profile/data/achievementsystem.h"
#include "../profile/scriptvars.h"

namespace Impacto {
namespace AchievementSystem {

using namespace Impacto::Profile::AchievementSystem;
using namespace Impacto::Profile::ScriptVars;

class AchievementFileLoader
    : public Loadable<AchievementFileLoader, AchievementError> {
  friend Loadable<AchievementFileLoader, AchievementError>;

 protected:
  void UnloadSync() {}
  AchievementError LoadSync() { return Implementation->MountAchievementFile(); }

  void MainThreadOnLoad(AchievementError result) {
    // Let's not report errors until we finalize the implementation
    result = AchievementError::OK;

    ScrWork[SW_SAVEERRORCODE] = (int)result;
  }
};

inline AchievementFileLoader Loader;

void Init() { Impacto::Profile::AchievementSystem::Configure(); }

AchievementError MountAchievementFile() {
  AchievementError result = AchievementError::InProgress;

  if (!Loader.LoadAsync()) {
    result = AchievementError::Failed;
  }

  ScrWork[SW_SAVEERRORCODE] = (int)result;
  return result;
}

const Achievement* GetAchievement(int id) {
  if (!Implementation) return nullptr;
  return Implementation->GetAchievement(id);
}

size_t GetAchievementCount() {
  if (!Implementation) return 0;
  return Implementation->GetAchievementCount();
}
}  // namespace AchievementSystem
}  // namespace Impacto