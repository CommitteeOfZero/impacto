#include "tipssystem.h"

#include "../profile/data/tipssystem.h"
#include <vector>

namespace Impacto {
namespace TipsSystem {

using namespace Impacto::Profile::TipsSystem;

void Init() { Configure(); }

void DataInit(int scriptBufferId, uint8_t* tipsData) {
  if (Implementation) Implementation->DataInit(scriptBufferId, tipsData);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: tips system not implemented, doing nothing\n", __func__);
}

void UpdateTipRecords() {
  if (Implementation) Implementation->UpdateTipRecords();
  ImpLog(LL_Warning, LC_VMStub,
         "%s: tips system not implemented, doing nothing\n", __func__);
}

void SetTipLockedState(int id, bool state) {
  if (Implementation) Implementation->SetTipLockedState(id, state);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: tips system not implemented, doing nothing\n", __func__);
}

void SetTipUnreadState(int id, bool state) {
  if (Implementation) Implementation->SetTipUnreadState(id, state);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: tips system not implemented, doing nothing\n", __func__);
}

void SetTipNewState(int id, bool state) {
  if (Implementation) Implementation->SetTipNewState(id, state);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: tips system not implemented, doing nothing\n", __func__);
}

bool GetTipLockedState(int id) {
  if (Implementation) return Implementation->GetTipLockedState(id);
  ImpLog(LL_Warning, LC_VMStub,
         "%s: tips system not implemented, returning false\n", __func__);
  return false;
}

std::vector<TipsDataRecord>* GetTipRecords() {
  if (Implementation) return &Implementation->Records;
  ImpLog(LL_Warning, LC_VMStub,
         "%s: tips system not implemented, returning NULL\n", __func__);
  return nullptr;
}

TipsDataRecord* GetTipRecord(int id) {
  if (Implementation) return &Implementation->Records[id];
  ImpLog(LL_Warning, LC_VMStub,
         "%s: tips system not implemented, returning NULL\n", __func__);
  return nullptr;
}

int GetTipCount() {
  if (Implementation) return Implementation->TipEntryCount;
  ImpLog(LL_Warning, LC_VMStub,
         "%s: tips system not implemented, returning 0\n", __func__);
  return 0;
}

}  // namespace TipsSystem
}  // namespace Impacto