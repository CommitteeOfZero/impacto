#include "tipssystem.h"

#include "../profile/data/tipssystem.h"

namespace Impacto {
namespace TipsSystem {

using namespace Impacto::Profile::TipsSystem;

TipsSystemBase* Implementation = 0;

void Init() { Configure(); }

void DataInit(int scriptBufferId, uint8_t* tipsData) {
  if (Implementation) Implementation->DataInit(scriptBufferId, tipsData);
}

void UpdateTipRecords() {
  if (Implementation) Implementation->UpdateTipRecords();
}

void SetTipLockedState(int id, bool state) {
  if (Implementation) Implementation->SetTipLockedState(id, state);
}

void SetTipUnreadState(int id, bool state) {
  if (Implementation) Implementation->SetTipUnreadState(id, state);
}

void SetTipNewState(int id, bool state) {
  if (Implementation) Implementation->SetTipNewState(id, state);
}

bool GetTipLockedState(int id) {
  if (Implementation) return Implementation->GetTipLockedState(id);
}

TipsDataRecord* GetTipRecords() {
  if (Implementation)
    return Implementation->Records;
  else
    return 0;
}

TipsDataRecord* GetTipRecord(int id) {
  if (Implementation)
    return &Implementation->Records[id];
  else
    return 0;
}

int GetTipCount() {
  if (Implementation)
    return Implementation->TipEntryCount;
  else
    return 0;
}

}  // namespace TipsSystem
}  // namespace Impacto