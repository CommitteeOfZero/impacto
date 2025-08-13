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
  auto scriptBuffer = ScriptBuffers[scriptBufferId];

  int idx = 0;
  // Read tips data from the script and create UI elements for each tip
  MemoryStream *stream =
      new MemoryStream(&scriptBuffer[tipsDataAdr], tipsDataSize);
  auto numberOfContentStrings = ReadLE<uint16_t>(stream);
  while (numberOfContentStrings != 255) {
    // Read tip entry from the data array
    TipsDataRecord record{.IsLocked = false, .IsUnread = false, .IsNew = false};
    record.Id = (uint16_t)idx;
    // TODO: record.SortLetterIndex
    record.NumberOfContentStrings = numberOfContentStrings;
    for (int i = 0; i < record.NumberOfContentStrings + 4; i++) {
      record.StringAdr[i] =
          ScriptGetStrAddress(scriptBufferId, ReadLE<uint16_t>(stream));
    }
    Records[idx] = record;

    // Next tip entry from the data array
    numberOfContentStrings = Io::ReadLE<uint16_t>(stream);
    idx += 1;
    TipEntryCount = idx;
  }

  // UI::TipsMenuPtr->Init();

  delete stream;
}

void TipsSystem::UpdateTipRecords() {
  if (TipEntryCount != 0) {
    for (int i = 0; i < TipEntryCount; i++) {
      auto record = &Records[i];
      auto tipStatus = SaveSystem::GetTipStatus(record->Id);
      record->IsLocked = (tipStatus & 1) == 0;
      record->IsUnread = (tipStatus & 2) == 0;
      record->IsNew = (tipStatus & 4) == 0;
    }
  }
}

void TipsSystem::SetTipLockedState(int id, bool state) { return; }
void TipsSystem::SetTipUnreadState(int id, bool state) { return; }
void TipsSystem::SetTipNewState(int id, bool state) { return; }

bool TipsSystem::GetTipLockedState(int id) { return Records[id].IsLocked; }

}  // namespace CHLCC
}  // namespace Impacto