#include "tipssystem.h"

#include "../profile/data/tipssystem.h"
#include "../profile/vm.h"
#include "../vm/vm.h"
#include "../vm/sc3stream.h"
#include "../text/text.h"
#include <vector>

namespace Impacto {
namespace TipsSystem {

using namespace Impacto::Profile::TipsSystem;

TipsComparator::TipsComparator(uint32_t tipsTableId, uint32_t sortStringIndex,
                               int tipIdStrIndex)
    : TipIdStrIndex(tipIdStrIndex) {
  auto [buffers, bufferId, ip] =
      Vm::ScriptGetTextTableStrAddress(tipsTableId, sortStringIndex);
  Vm::Sc3Stream sortStream = &buffers[bufferId][ip];

  int distance = 0;

  while (sortStream.PeekU8() != 0xff) {
    StringToken token;
    token.Read(sortStream);
    if (token.Type == STT_Character) {
      Sc3SortMap[token.Val_Int] = distance++;
    } else {
      ImpLogSlow(LogLevel::Warning, LogChannel::VM,
                 "TipsComparator: SC3 Tag Found in Sort String\n", token.Type);
    }
  }
}

bool TipsComparator::operator()(int a, int b) const {
  TipsBufferCtx bufferCtx = TipsSystem::GetTipsScriptBufferCtx();
  Vm::Sc3Stream aString = TipsSystem::GetTextStringStream(a, TipIdStrIndex);
  Vm::Sc3Stream bString = TipsSystem::GetTextStringStream(b, TipIdStrIndex);

  while (aString.PeekU8() != 0xff && bString.PeekU8() != 0xff) {
    StringToken aToken;
    StringToken bToken;
    aToken.Read(aString);
    bToken.Read(bString);

    if (aToken.Val_Int != bToken.Val_Int) {
      auto aSortValue = Sc3SortMap.find(aToken.Val_Int);
      auto bSortValue = Sc3SortMap.find(bToken.Val_Int);
      if (aSortValue != Sc3SortMap.end() && bSortValue != Sc3SortMap.end()) {
        return aSortValue->second < bSortValue->second;
      } else {
        ImpLogSlow(LogLevel::Error, LogChannel::VM,
                   "TipsComparator: Character Not Found in Sort String\n",
                   aToken.Val_Int, bToken.Val_Int);
        return aToken.Val_Int < bToken.Val_Int;
      }
    }
  }
  // If strings are all the same, return the shorter one
  return aString.PeekU8() == 0xff && bString.PeekU8() != 0xff;
}

void Init() { Configure(); }

Vm::Sc3Stream GetTextStringStream(size_t record, size_t stringIndex) {
  if (!Implementation) return {nullptr};
  auto [buffers, bufferId] = GetTipsScriptBufferCtx();
  auto const& records = *GetTipRecords();
  return Vm::Sc3Stream{
      &buffers[bufferId][records[record].StringAdr[stringIndex]]};
}

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

TipsBufferCtx GetTipsScriptBufferCtx() {
  if (Implementation)
    return {.Buffers =
                Profile::Vm::UseMsbStrings ? Vm::MsbBuffers : Vm::ScriptBuffers,
            .BufferId = Implementation->ScriptBufferId};
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, returning empty ctx\n", __func__);
  return TipsBufferCtx{};
}

std::vector<uint16_t>& GetNewTipsIndices() {
  if (Implementation) return Implementation->NewTipsIndices;
  ImpLog(LogLevel::Warning, LogChannel::VMStub,
         "{:s}: tips system not implemented, returning empty vector\n",
         __func__);
  static std::vector<uint16_t> empty;
  return empty;
}
}  // namespace TipsSystem
}  // namespace Impacto