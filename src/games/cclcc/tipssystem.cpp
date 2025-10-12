#include "tipssystem.h"

#include "../../data/savesystem.h"
#include "../../vm/vm.h"
#include "../../io/memorystream.h"
#include "../../profile/data/tipssystem.h"
#include "../../ui/ui.h"

namespace Impacto {
namespace CCLCC {

using namespace Impacto::Vm;
using namespace Impacto::Profile::TipsSystem;
using namespace Impacto::Io;

void TipsSystem::DataInit(uint32_t scriptBufferId, uint32_t tipsDataAdr,
                          uint32_t tipsDataSize) {
  ScriptBufferId = (uint8_t)scriptBufferId;
  auto scriptBuffer = ScriptBuffers[scriptBufferId];

  TipEntryCount = 0;

  // Read tips data from the script and create UI elements for each tip
  MemoryStream stream =
      MemoryStream(&scriptBuffer[tipsDataAdr], tipsDataSize, false);
  int numberOfContentStrings = ReadLE<uint16_t>(&stream);
  while (numberOfContentStrings != 255) {
    if (TipEntryCount >= MaxTipsCount) {
      ImpLog(LogLevel::Error, LogChannel::VM, "Too many tips in tips data\n");
      break;
    }
    // Read tip entry from the data array
    TipsDataRecord record;
    record.Id = (uint16_t)TipEntryCount;
    record.NumberOfContentStrings = (uint16_t)numberOfContentStrings;
    for (int i = 0; i < numberOfContentStrings + 4; i++) {
      record.StringAdr[i] =
          ScriptGetStrAddress(scriptBufferId, ReadLE<uint16_t>(&stream));
    }
    Records[TipEntryCount] = std::move(record);

    // Next tip entry from the data array
    numberOfContentStrings = ReadLE<uint16_t>(&stream);
    TipEntryCount += 1;
  }
  Records.resize(TipEntryCount);
}

void TipsSystem::UpdateTipRecords() {
  if (TipEntryCount != 0) {
    for (size_t i = 0; i < TipEntryCount; i++) {
      auto &record = Records[i];
      auto tipStatus = SaveSystem::GetTipStatus(record.Id);
      record.IsLocked = (tipStatus & 1) == 0;
      record.IsUnread = (tipStatus & 2) == 0;
      record.IsNew = (tipStatus & 4) == 0;
    }
  }
}

void TipsSystem::SetTipLockedState(size_t id, bool state) {
  Records[id].IsLocked = state;
  SaveSystem::SetTipStatus(id, Records[id].IsLocked, Records[id].IsUnread,
                           Records[id].IsNew);
}

void TipsSystem::SetTipUnreadState(size_t id, bool state) {
  Records[id].IsUnread = state;
  SaveSystem::SetTipStatus(id, Records[id].IsLocked, Records[id].IsUnread,
                           Records[id].IsNew);
}

void TipsSystem::SetTipNewState(size_t id, bool state) {
  Records[id].IsNew = state;
  SaveSystem::SetTipStatus(id, Records[id].IsLocked, Records[id].IsUnread,
                           Records[id].IsNew);
}

bool TipsSystem::GetTipLockedState(size_t id) { return Records[id].IsLocked; }

}  // namespace CCLCC
}  // namespace Impacto