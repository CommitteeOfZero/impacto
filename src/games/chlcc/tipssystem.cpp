#include "tipssystem.h"

#include "../../data/savesystem.h"
#include "../../vm/vm.h"
#include "../../io/memorystream.h"
#include "../../profile/data/tipssystem.h"
#include "../../ui/ui.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::Vm;
using namespace Impacto::Profile::TipsSystem;
using namespace Impacto::Io;

void TipsSystem::DataInit(uint32_t scriptBufferId, uint32_t tipsDataAdr,
                          uint32_t tipsDataSize) {
  ScriptBufferId = (uint8_t)scriptBufferId;
  const auto scriptBuffer = ScriptBuffers[scriptBufferId];

  // Read tips data from the script and create UI elements for each tip
  MemoryStream stream =
      MemoryStream(&scriptBuffer[tipsDataAdr], tipsDataSize, false);
  uint16_t numberOfContentStrings = ReadLE<uint16_t>(&stream);
  while (numberOfContentStrings != 255) {
    if (TipEntryCount >= MaxTipsCount) {
      ImpLog(LogLevel::Error, LogChannel::VM, "Too many tips in tips data\n");
      break;
    }

    // Read tip entry from the data array
    TipsDataRecord record{
        .Id = static_cast<uint16_t>(TipEntryCount),
        // TODO: .SortLetterIndex
        .NumberOfContentStrings = numberOfContentStrings,
        .IsLocked = true,
        .IsUnread = true,
        .IsNew = true,
    };
    for (uint16_t i = 0; i < record.NumberOfContentStrings + 4; i++) {
      record.StringAdr[i] =
          ScriptGetStrAddress(scriptBufferId, ReadLE<uint16_t>(&stream));
    }
    Records[TipEntryCount] = std::move(record);

    // Next tip entry from the data array
    numberOfContentStrings = Io::ReadLE<uint16_t>(&stream);
    TipEntryCount++;
  }

  Records.resize(TipEntryCount);
  // UI::TipsMenuPtr->Init();
}

void TipsSystem::UpdateTipRecords() {
  if (TipEntryCount != 0) {
    for (size_t i = 0; i < TipEntryCount; i++) {
      TipsDataRecord& record = Records[i];
      const uint32_t tipStatus = SaveSystem::GetTipStatus(record.Id);
      record.IsLocked = (tipStatus & 1) == 0;
      record.IsUnread = (tipStatus & 2) == 0;
      record.IsNew = (tipStatus & 4) == 0;
    }
  }
}

void TipsSystem::SetTipLockedState(int id, bool state) {
  Records[id].IsLocked = state;
  SaveSystem::SetTipStatus(id, Records[id].IsLocked, Records[id].IsUnread,
                           Records[id].IsNew);
}

void TipsSystem::SetTipUnreadState(int id, bool state) {
  Records[id].IsUnread = state;
  SaveSystem::SetTipStatus(id, Records[id].IsLocked, Records[id].IsUnread,
                           Records[id].IsNew);
}

void TipsSystem::SetTipNewState(int id, bool state) {
  Records[id].IsNew = state;
  SaveSystem::SetTipStatus(id, Records[id].IsLocked, Records[id].IsUnread,
                           Records[id].IsNew);
}

bool TipsSystem::GetTipLockedState(int id) { return Records[id].IsLocked; }

}  // namespace CHLCC
}  // namespace Impacto