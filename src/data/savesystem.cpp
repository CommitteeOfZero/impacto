#include "savesystem.h"

#include "../io/physicalfilestream.h"
#include "../mem.h"
#include "../vm/vm.h"
#include "../profile/data/savesystem.h"

namespace Impacto {
namespace SaveSystem {

using namespace Impacto::Vm;
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

uint8_t GetSaveSatus(SaveType type, int id) {
  if (Implementation) return Implementation->GetSaveSatus(type, id);
}

int GetSaveTitle(SaveType type, int id) {
  if (Implementation) return Implementation->GetSaveTitle(type, id);
}

uint32_t GetTipStatus(int tipId) {
  if (Implementation) return Implementation->GetTipStatus(tipId);
}

void SetTipStatus(int tipId, bool isLocked, bool isUnread, bool isNew) {
  if (Implementation)
    Implementation->SetTipStatus(tipId, isLocked, isUnread, isNew);
}

void GetReadMessagesCount(int* totalMessageCount, int* readMessageCount) {
  if (Implementation)
    Implementation->GetReadMessagesCount(totalMessageCount, readMessageCount);
}

void GetViewedEVsCount(int* totalEVCount, int* viewedEVCount) {
  if (Implementation)
    Implementation->GetViewedEVsCount(totalEVCount, viewedEVCount);
}

void GetEVStatus(int evId, int* totalVariations, int* viewedVariations) {
  if (Implementation)
    Implementation->GetEVStatus(evId, totalVariations, viewedVariations);
}

bool GetBgmFlag(int id) {
  if (Implementation) return Implementation->GetBgmFlag(id);
}

}  // namespace SaveSystem
}  // namespace Impacto