#include "savesystem.h"

#include "../profile/data/savesystem.h"
#include "../profile/scriptvars.h"
#include "../profile/vm.h"
#include "../mem.h"

#include <cstdint>
#include <ctime>

namespace Impacto {
namespace SaveSystem {

using namespace Impacto::Profile::SaveSystem;
using namespace Impacto::Profile::ScriptVars;

void Init() { Impacto::Profile::SaveSystem::Configure(); }

class SaveFileLoader : public Loadable<SaveFileLoader, SaveError> {
  friend Loadable<SaveFileLoader, SaveError>;

 protected:
  void UnloadSync() {}
  SaveError LoadSync() {
    return Implementation ? Implementation->MountSaveFile(QueuedTextures)
                          : SaveError::Failed;
  }

  void MainThreadOnLoad(SaveError result) {
    // Texture submission has to happen on the main thread
    for (QueuedTexture& texture : QueuedTextures) {
      texture.Id.get() = texture.Tex.Submit();
    }

    QueuedTextures.clear();

    // Let's not report errors until we finalize the implementation
    if (Profile::Vm::GameInstructionSet != +Vm::InstructionSet::CC &&
        Profile::Vm::GameInstructionSet != +Vm::InstructionSet::CHLCC) {
      result = SaveError::OK;
    }

    ScrWork[SW_SAVEERRORCODE] = (int)result;
  }

 private:
  std::vector<QueuedTexture> QueuedTextures;
};

class SaveFileChecker : public Loadable<SaveFileChecker, SaveError> {
  friend Loadable<SaveFileChecker, SaveError>;

 protected:
  void UnloadSync() {}
  SaveError LoadSync() {
    return Implementation ? Implementation->CheckSaveFile() : SaveError::Failed;
  }

  void MainThreadOnLoad(SaveError result) {
    // Let's not report errors until we finalize the implementation
    if (Profile::Vm::GameInstructionSet != +Vm::InstructionSet::CC &&
        Profile::Vm::GameInstructionSet != +Vm::InstructionSet::CHLCC) {
      result = SaveError::OK;
    }

    ScrWork[SW_SAVEERRORCODE] = (int)result;
  }
};

class SaveFileWriter : public Loadable<SaveFileWriter, SaveError> {
  friend Loadable<SaveFileWriter, SaveError>;

 protected:
  void UnloadSync() {}
  SaveError LoadSync() {
    return Implementation ? Implementation->WriteSaveFile() : SaveError::Failed;
  }

  void MainThreadOnLoad(SaveError result) {
    // Let's not report errors until we finalize the implementation
    if (Profile::Vm::GameInstructionSet != +Vm::InstructionSet::CC &&
        Profile::Vm::GameInstructionSet != +Vm::InstructionSet::CHLCC) {
      result = SaveError::OK;
    }

    ScrWork[SW_SAVEERRORCODE] = (int)result;
  }
};

static std::variant<SaveFileLoader, SaveFileChecker, SaveFileWriter> Loader;

template <typename TLoader>
void ExecuteLoader() {
  SaveError result = Loader.emplace<TLoader>().LoadAsync()
                         ? SaveError::InProgress
                         : SaveError::Failed;

  ScrWork[SW_SAVEERRORCODE] = (int)result;
}

LoadStatus GetLoadStatus() {
  return std::visit([](auto& loader) -> LoadStatus { return loader.Status; },
                    Loader);
}

void MountSaveFile() { ExecuteLoader<SaveFileLoader>(); }
void CheckSaveFile() { ExecuteLoader<SaveFileChecker>(); }
void WriteSaveFile() { ExecuteLoader<SaveFileWriter>(); }

void InitializeSystemData() {
  if (Implementation) Implementation->InitializeSystemData();
}

void SaveSystemData() {
  if (Implementation) Implementation->SaveSystemData();
}

SaveError LoadSystemData() {
  return Implementation ? Implementation->LoadSystemData() : SaveError::Failed;
}

void SaveThumbnailData() {
  if (Implementation) Implementation->SaveThumbnailData();
}

void SaveMemory() {
  if (Implementation) Implementation->SaveMemory();
}

void LoadEntry(SaveType type, int id) {
  if (Implementation) Implementation->LoadEntry(type, id);
}

void LoadMemoryNew(LoadProcess load) {
  if (Implementation) Implementation->LoadMemoryNew(load);
}

void FlushWorkingSaveEntry(SaveType type, int id, int autoSaveType) {
  if (Implementation)
    Implementation->FlushWorkingSaveEntry(type, id, autoSaveType);
}

uint32_t GetSavePlayTime(SaveType type, int id) {
  if (Implementation) return Implementation->GetSavePlayTime(type, id);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returning 0\n", __func__);
  return 0;
}

uint8_t GetSaveFlags(SaveType type, int id) {
  if (Implementation) return Implementation->GetSaveFlags(type, id);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returning 0\n", __func__);
  return 0;
}

void SetSaveFlags(SaveType type, int id, uint8_t flags) {
  if (Implementation) return Implementation->SetSaveFlags(type, id, flags);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returning 0\n", __func__);
}

tm const& GetSaveDate(SaveType type, int id) {
  const static tm t = [] {
    tm tmStruct{};
    tmStruct.tm_mday = 1;
    return tmStruct;
  }();

  if (Implementation) return Implementation->GetSaveDate(type, id);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returning dummy time\n", __func__);
  return t;
}

uint8_t GetSaveStatus(SaveType type, int id) {
  if (Implementation) return Implementation->GetSaveStatus(type, id);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returning 0\n", __func__);
  return 0;
}

int GetSaveTitle(SaveType type, int id) {
  if (Implementation) return Implementation->GetSaveTitle(type, id);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returning 0\n", __func__);
  return 0;
}

uint32_t GetTipStatus(size_t tipId) {
  if (Implementation) return Implementation->GetTipStatus(tipId);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returning 0\n", __func__);
  return 0;
}

void SetTipStatus(size_t tipId, bool isLocked, bool isUnread, bool isNew) {
  if (Implementation)
    return Implementation->SetTipStatus(tipId, isLocked, isUnread, isNew);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented\n", __func__);
}

void SetLineRead(int scriptId, int lineId) {
  if (Implementation) return Implementation->SetLineRead(scriptId, lineId);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented\n", __func__);
}

bool IsLineRead(int scriptId, int lineId) {
  if (Implementation) return Implementation->IsLineRead(scriptId, lineId);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returing false\n", __func__);
  return false;
}

void GetReadMessagesCount(int* totalMessageCount, int* readMessageCount) {
  if (Implementation)
    return Implementation->GetReadMessagesCount(totalMessageCount,
                                                readMessageCount);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returning 0\n", __func__);
  *totalMessageCount = 0;
  *readMessageCount = 0;
}

void GetViewedEVsCount(int* totalEVCount, int* viewedEVCount) {
  if (Implementation)
    return Implementation->GetViewedEVsCount(totalEVCount, viewedEVCount);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returning 0\n", __func__);
  *totalEVCount = 0;
  *viewedEVCount = 0;
}

void GetEVStatus(int evId, int* totalVariations, int* viewedVariations) {
  if (Implementation)
    return Implementation->GetEVStatus(evId, totalVariations, viewedVariations);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returning 0\n", __func__);
  *totalVariations = 0;
  *viewedVariations = 0;
}

void SetEVStatus(int id) {
  if (Implementation) return Implementation->SetEVStatus(id);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented\n", __func__);
}

bool GetEVVariationIsUnlocked(size_t evId, size_t variationIdx) {
  if (Implementation)
    return Implementation->GetEVVariationIsUnlocked(evId, variationIdx);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returing false\n", __func__);
  return false;
}

bool GetBgmFlag(int id) {
  if (Implementation) return Implementation->GetBgmFlag(id);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returing false\n", __func__);
  return false;
}

void SetBgmFlag(int id, bool flag) {
  if (Implementation) {
    Implementation->SetBgmFlag(id, flag);
    return;
  }
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented\n", __func__);
}

void SetCheckpointId(int id) {
  if (Implementation) Implementation->SetCheckpointId(id);
}

int GetQuickSaveOpenSlot() {
  if (Implementation) return Implementation->GetQuickSaveOpenSlot();
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returning -1\n", __func__);
  return -1;
}

Sprite& GetSaveThumbnail(SaveType type, int id) {
  if (Implementation) return Implementation->GetSaveThumbnail(type, id);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returning dummy sprite\n",
         __func__);
  static Sprite dummy;
  return dummy;
}

Sprite& GetWorkingSaveThumbnail() {
  if (Implementation) return Implementation->GetWorkingSaveThumbnail();
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returning dummy sprite\n",
         __func__);
  static Sprite dummy;
  return dummy;
}

bool HasQSavedOnCurrentLine() {
  if (Implementation) return Implementation->HasQSavedOnCurrentLine();
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returning -1\n", __func__);
  return false;
}

void SetQSavedOnCurrentLine(bool value) {
  if (Implementation) Implementation->SetQSavedOnCurrentLine(value);
}

int SaveSystemBase::GetLockedQuickSaveCount() { return LockedQuickSaveCount; }

void SaveSystemBase::SetLockedQuickSaveCount(int value) {
  LockedQuickSaveCount = value;
}

bool SaveSystemBase::HasQSavedOnCurrentLine() const {
  return QuickSavedOnCurrentLine;
}

void SaveSystemBase::SetQSavedOnCurrentLine(bool value) {
  QuickSavedOnCurrentLine = value;
}

}  // namespace SaveSystem
}  // namespace Impacto