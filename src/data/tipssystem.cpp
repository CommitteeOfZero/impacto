#include "tipssystem.h"

#include "../profile/data/tipssystem.h"
#include <vector>

namespace Impacto {
namespace TipsSystem {

using namespace Impacto::Profile::TipsSystem;

void Init() { Configure(); }

void DataInit(uint32_t scriptBufferId, uint32_t tipsDataAdr,
              uint32_t tipsDataSize) {
  if (Implementation)
    return Implementation->DataInit(scriptBufferId, tipsDataAdr, tipsDataSize);

  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, doing nothing\n", __func__);
}

void UpdateTipRecords() {
  if (Implementation) return Implementation->UpdateTipRecords();
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, doing nothing\n", __func__);
}

void SetTipLockedState(int id, bool state) {
  if (Implementation) return Implementation->SetTipLockedState(id, state);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, doing nothing\n", __func__);
}

void SetTipUnreadState(int id, bool state) {
  if (Implementation) return Implementation->SetTipUnreadState(id, state);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, doing nothing\n", __func__);
}

void SetTipNewState(int id, bool state) {
  if (Implementation) return Implementation->SetTipNewState(id, state);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, doing nothing\n", __func__);
}

bool GetTipLockedState(int id) {
  if (Implementation) return Implementation->GetTipLockedState(id);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, returning false\n", __func__);
  return false;
}

std::vector<TipsDataRecord>* GetTipRecords() {
  if (Implementation) return &Implementation->Records;
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, returning NULL\n", __func__);
  return nullptr;
}

TipsDataRecord* GetTipRecord(int id) {
  if (Implementation) return &Implementation->Records[id];
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, returning NULL\n", __func__);
  return nullptr;
}

int GetTipCount() {
  if (Implementation) return Implementation->TipEntryCount;
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, returning 0\n", __func__);
  return 0;
}

uint8_t GetTipsScriptBufferId() {
  if (Implementation) return Implementation->ScriptBufferId;
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, returning 0\n", __func__);
  return 0;
}
}  // namespace TipsSystem
}  // namespace Impacto