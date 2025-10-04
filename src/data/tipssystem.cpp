#include "tipssystem.h"

#include "../profile/data/tipssystem.h"
#include "../vm/vm.h"
#include <vector>

namespace Impacto {
namespace TipsSystem {

using namespace Impacto::Profile::TipsSystem;

TipsComparator::TipsComparator(uint32_t tipsTableId, uint32_t sortStringIndex,
                               int tipIdStrIndex)
    : SortString(nullptr), TipIdStrIndex(tipIdStrIndex) {
  auto [scrBufId, offset] =
      Vm::ScriptGetTextTableStrAddress(tipsTableId, sortStringIndex);
  SortString = &Vm::ScriptBuffers[scrBufId][offset];
  int i = 0;
  int distance = 0;
  while (SortString[i] != 0xFF) {
    if (SortString[i] & 0x80) {
      uint16_t sc3Char = SDL_SwapBE16(UnalignedRead<uint16_t>(SortString + i));
      Sc3SortMap[sc3Char] = distance++;
      i += 2;
    } else {
      ImpLogSlow(LogLevel::Warning, LogChannel::VM,
                 "TipsComparator: SC3 Tag Found in Sort String\n",
                 SortString[i]);
      i++;
    }
  }
}

bool TipsComparator::operator()(int a, int b) const {
  auto* aRecord = TipsSystem::GetTipRecord(a);
  auto* bRecord = TipsSystem::GetTipRecord(b);
  uint32_t tipsScrBufId = TipsSystem::GetTipsScriptBufferId();
  uint8_t* aString =
      &Vm::ScriptBuffers[tipsScrBufId][aRecord->StringAdr[TipIdStrIndex]];
  uint8_t* bString =
      &Vm::ScriptBuffers[tipsScrBufId][bRecord->StringAdr[TipIdStrIndex]];

  int aIndex = 0;
  int bIndex = 0;

  while (aString[aIndex] != 0xff && bString[bIndex] != 0xff) {
    if ((aString[aIndex] & 0x80) == 0) {
      aIndex++;
      continue;
    }
    if ((bString[bIndex] & 0x80) == 0) {
      bIndex++;
      continue;
    }
    uint16_t aSc3Char = SDL_SwapBE16(UnalignedRead<uint16_t>(aString + aIndex));
    aIndex += 2;

    uint16_t bSc3Char = SDL_SwapBE16(UnalignedRead<uint16_t>(bString + bIndex));
    bIndex += 2;
    if (aSc3Char != bSc3Char) {
      auto aSortValue = Sc3SortMap.find(aSc3Char);
      auto bSortValue = Sc3SortMap.find(bSc3Char);
      if (aSortValue != Sc3SortMap.end() && bSortValue != Sc3SortMap.end()) {
        return aSortValue->second < bSortValue->second;
      } else {
        ImpLogSlow(LogLevel::Error, LogChannel::VM,
                   "TipsComparator: Character Not Found in Sort String\n",
                   aSc3Char, bSc3Char);
        return aSc3Char < bSc3Char;
      }
    }
  }
  // If strings are all the same, return the shorter one
  return aString[aIndex] == 0xff && bString[bIndex] != 0xff;
}

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

void SetTipLockedState(size_t id, bool state) {
  if (Implementation) return Implementation->SetTipLockedState(id, state);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, doing nothing\n", __func__);
}

void SetTipUnreadState(size_t id, bool state) {
  if (Implementation) return Implementation->SetTipUnreadState(id, state);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, doing nothing\n", __func__);
}

void SetTipNewState(size_t id, bool state) {
  if (Implementation) return Implementation->SetTipNewState(id, state);
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, doing nothing\n", __func__);
}

bool GetTipLockedState(size_t id) {
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

TipsDataRecord* GetTipRecord(size_t id) {
  if (Implementation) return &Implementation->Records[id];
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, returning NULL\n", __func__);
  return nullptr;
}

size_t GetTipCount() {
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