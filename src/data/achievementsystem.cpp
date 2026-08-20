#include "achievementsystem.h"

#include "../mem.h"
#include "../profile/data/achievementsystem.h"
#include "../profile/scriptvars.h"
#include "../io/physicalfilestream.h"
#include "../log.h"

namespace Impacto {
namespace AchievementSystem {

using namespace Impacto::Profile::AchievementSystem;
using namespace Impacto::Profile::ScriptVars;

void Init() {
  Profile::AchievementSystem::Configure();
  if (Implementation) {
    MountAchievementFile();
  }
}

class AchievementFileLoader
    : public Loadable<AchievementFileLoader, AchievementError> {
  friend Loadable<AchievementFileLoader, AchievementError>;

 protected:
  void UnloadSync() {}
  AchievementError LoadSync() {
    if (Implementation) {
      return Implementation->MountAchievementFile(MainThreadCallback);
    }

    return AchievementError::Failed;
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

static std::vector<uint8_t> UnlockState;
static bool UnlockStateLoaded = false;

static void LoadUnlockState() {
  UnlockState.clear();
  UnlockStateLoaded = true;
  if (AchievementDataPath.empty()) return;

  Io::Stream* stream;
  if (Io::PhysicalFileStream::Create(AchievementDataPath, &stream) !=
      IoError_OK) {
    return;
  }
  UnlockState.resize(stream->Meta.Size);
  Io::ReadArrayWithoutSwap<uint8_t>(UnlockState.data(), stream,
                                    UnlockState.size());
  delete stream;
}

static void SaveUnlockState() {
  if (AchievementDataPath.empty()) return;

  using CF = Io::PhysicalFileStream::CreateFlagsMode;
  Io::Stream* stream;
  IoError err = Io::PhysicalFileStream::Create(
      AchievementDataPath, &stream,
      CF::CREATE | CF::CREATE_DIRS | CF::WRITE | CF::TRUNCATE);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Couldn't write achievement unlock data {:s}\n",
           AchievementDataPath);
    return;
  }
  Io::WriteArrayWithoutSwap<uint8_t>(UnlockState.data(), stream,
                                     UnlockState.size());
  delete stream;
}

bool IsAchievementUnlocked(int id) {
  if (!UnlockStateLoaded) LoadUnlockState();
  if (id < 0 || (size_t)id >= UnlockState.size()) return false;
  return UnlockState[id] != 0;
}

bool UnlockAchievement(int id) {
  if (!UnlockStateLoaded) LoadUnlockState();
  if (id < 0) return false;

  if ((size_t)id >= UnlockState.size()) UnlockState.resize(id + 1, 0);
  if (UnlockState[id] != 0) return false;

  UnlockState[id] = 1;
  SaveUnlockState();
  return true;
}

}  // namespace AchievementSystem
}  // namespace Impacto