#include "savesystem.h"

#include "../profile/data/savesystem.h"

#include <cstdint>
#include <ctime>

namespace Impacto {
namespace SaveSystem {

using namespace Impacto::Profile::SaveSystem;

void Init() { Configure(); }

SaveError CreateSaveFile() {
  if (Implementation)
    return Implementation->CreateSaveFile();
  else
    return SaveError::OK;  // Just so we don't get stuck
}

SaveError CheckSaveFile() {
  if (Implementation)
    return Implementation->CheckSaveFile();
  else
    return SaveError::OK;
}

SaveError MountSaveFile() {
  if (Implementation)
    return Implementation->MountSaveFile();
  else
    return SaveError::OK;  // Just so we don't get stuck at loading save data in
                           // script.
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

void WriteSaveFile() {
  if (Implementation) Implementation->WriteSaveFile();
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

tm const& GetSaveDate(SaveType type, int id) {
  static tm t = []() {
    tm t;
    t.tm_sec = 0;
    t.tm_min = 0;
    t.tm_hour = 0;
    t.tm_mday = 1;
    t.tm_mon = 0;
    t.tm_year = 0;
    return t;
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

uint32_t GetTipStatus(int tipId) {
  if (Implementation) return Implementation->GetTipStatus(tipId);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: save system not implemented, returning 0\n", __func__);
  return 0;
}

void SetTipStatus(int tipId, bool isLocked, bool isUnread, bool isNew) {
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

bool GetEVVariationIsUnlocked(int evId, int variationIdx) {
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

}  // namespace SaveSystem
}  // namespace Impacto