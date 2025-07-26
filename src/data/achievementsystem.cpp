#include "achievementsystem.h"

#include "../mem.h"
#include "../profile/data/achievementsystem.h"
#include "../profile/scriptvars.h"

namespace Impacto {
namespace AchievementSystem {

using namespace Impacto::Profile::AchievementSystem;
using namespace Impacto::Profile::ScriptVars;

void Init() { Impacto::Profile::AchievementSystem::Configure(); }

class AchievementFileLoader
    : public Loadable<AchievementFileLoader, AchievementError> {
  friend Loadable<AchievementFileLoader, AchievementError>;

 protected:
  void UnloadSync() {}
  AchievementError LoadSync() {
    return Implementation->MountAchievementFile(MainThreadCallback);
  }

  void MainThreadOnLoad(AchievementError result) {
    if (MainThreadCallback) {
      MainThreadCallback();
      MainThreadCallback = nullptr;
    }

    // Let's not report errors until we finalize the implementation
    result = AchievementError::OK;

    ScrWork[SW_SAVEERRORCODE] = (int)result;
  }

 private:
  std::function<void(void)> MainThreadCallback;
};

static AchievementFileLoader Loader;

LoadStatus GetLoadStatus() { return Loader.Status; }

void MountAchievementFile() {
  AchievementError result = Loader.LoadAsync() ? AchievementError::InProgress
                                               : AchievementError::Failed;

  ScrWork[SW_SAVEERRORCODE] = (int)result;
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