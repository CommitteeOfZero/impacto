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

void TipsSystem::DataInit(int scriptBufferId, uint8_t *tipsData,
                          uint32_t tipsDataSize) {
  ScriptBufferId = scriptBufferId;
  auto scriptBuffer = ScriptBuffers[scriptBufferId];

  TipEntryCount = 0;

  // Read tips data from the script and create UI elements for each tip
  MemoryStream stream = MemoryStream(tipsData, tipsDataSize, false);
  int numberOfContentStrings = ReadLE<uint16_t>(&stream);
  while (numberOfContentStrings != 255) {
    if (TipEntryCount >= MaxTipsCount) {
      ImpLog(LL_Error, LC_VM, "Too many tips in tips data\n");
      break;
    }
    // Read tip entry from the data array
    TipsDataRecord record;
    record.Id = TipEntryCount;
    // TODO: record.SortLetterIndex
    record.NumberOfContentStrings = numberOfContentStrings;
    for (int i = 0; i < numberOfContentStrings + 4; i++) {
      record.StringPtrs[i] =
          ScriptGetStrAddress(scriptBuffer, ReadLE<uint16_t>(&stream));
    }
    Records[TipEntryCount] = std::move(record);

    // Next tip entry from the data array
    numberOfContentStrings = ReadLE<uint16_t>(&stream);
    TipEntryCount += 1;
  }
  Records.resize(TipEntryCount);
  UI::TipsMenuPtr->Init();
}

void TipsSystem::UpdateTipRecords() {
  if (TipEntryCount != 0) {
    for (int i = 0; i < TipEntryCount; i++) {
      auto &record = Records[i];
      auto tipStatus = SaveSystem::GetTipStatus(record.Id);
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

}  // namespace CCLCC
}  // namespace Impacto