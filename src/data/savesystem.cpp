#include "savesystem.h"

#include "../profile/data/savesystem.h"

#include <cstdint>
#include <ctime>

namespace Impacto {
namespace SaveSystem {

using namespace Impacto::Profile::SaveSystem;

SaveSystemBase* Implementation = 0;

void Init() { Configure(); }

SaveError MountSaveFile() {
  if (Implementation)
    return Implementation->MountSaveFile();
  else
    return SaveOK;  // Just so we don't get stuck at loading save data in
                    // script.
}

void SaveMemory() {
  if (Implementation) Implementation->SaveMemory();
}

void LoadMemory(SaveType type, int id) {
  if (Implementation) Implementation->LoadMemory(type, id);
}

void FlushWorkingSaveEntry(SaveType type, int id) {
  if (Implementation) Implementation->FlushWorkingSaveEntry(type, id);
}

void WriteSaveFile() {
  if (Implementation) Implementation->WriteSaveFile();
}

uint32_t GetSavePlayTime(SaveType type, int id) {
  if (Implementation) return Implementation->GetSavePlayTime(type, id);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: save system not implemented, returning 0\n", __func__);
  return 0;
}

uint8_t GetSaveFlags(SaveType type, int id) {
  if (Implementation) return Implementation->GetSaveFlags(type, id);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: save system not implemented, returning 0\n", __func__);
  return 0;
}

tm GetSaveDate(SaveType type, int id) {
  if (Implementation) return Implementation->GetSaveDate(type, id);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: save system not implemented, returning dummy time\n", __func__);
  return *gmtime(0);
}

uint8_t GetSaveStatus(SaveType type, int id) {
  if (Implementation) return Implementation->GetSaveStatus(type, id);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: save system not implemented, returning 0\n", __func__);
  return 0;
}

int GetSaveTitle(SaveType type, int id) {
  if (Implementation) return Implementation->GetSaveTitle(type, id);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: save system not implemented, returning 0\n", __func__);
  return 0;
}

uint32_t GetTipStatus(int tipId) {
  if (Implementation) return Implementation->GetTipStatus(tipId);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: save system not implemented, returning 0\n", __func__);
  return 0;
}

void SetTipStatus(int tipId, bool isLocked, bool isUnread, bool isNew) {
  if (Implementation)
    return Implementation->SetTipStatus(tipId, isLocked, isUnread, isNew);
  ImpLog(LL_Warning, LC_VMStub, "%s: save system not implemented\n", __func__);
}

void GetReadMessagesCount(int* totalMessageCount, int* readMessageCount) {
  if (Implementation)
    return Implementation->GetReadMessagesCount(totalMessageCount,
                                                readMessageCount);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: save system not implemented, returning 0\n", __func__);
  *totalMessageCount = 0;
  *readMessageCount = 0;
}

void GetViewedEVsCount(int* totalEVCount, int* viewedEVCount) {
  if (Implementation)
    return Implementation->GetViewedEVsCount(totalEVCount, viewedEVCount);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: save system not implemented, returning 0\n", __func__);
  *totalEVCount = 0;
  *viewedEVCount = 0;
}

void GetEVStatus(int evId, int* totalVariations, int* viewedVariations) {
  if (Implementation)
    return Implementation->GetEVStatus(evId, totalVariations, viewedVariations);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: save system not implemented, returning 0\n", __func__);
  *totalVariations = 0;
  *viewedVariations = 0;
}

bool GetEVVariationIsUnlocked(int evId, int variationIdx) {
  if (Implementation)
    return Implementation->GetEVVariationIsUnlocked(evId, variationIdx);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: save system not implemented, returing false\n", __func__);
  return false;
}

bool GetBgmFlag(int id) {
  if (Implementation) return Implementation->GetBgmFlag(id);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: save system not implemented, returing false\n", __func__);
  return false;
}

void SetCheckpointId(int id) {
  if (Implementation) Implementation->SetCheckpointId(id);
}

}  // namespace SaveSystem
}  // namespace Impacto