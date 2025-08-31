#include "tipssystem.h"

#include "../../data/savesystem.h"
#include "../../vm/vm.h"
#include "../../io/memorystream.h"
#include "../../profile/data/tipssystem.h"
#include "../../ui/ui.h"

namespace Impacto {
namespace MO6TW {

using namespace Impacto::Vm;
using namespace Impacto::Profile::TipsSystem;
using namespace Impacto::Io;

void TipsSystem::DataInit(uint32_t scriptBufferId, uint32_t tipsDataAdr,
                          uint32_t tipsDataSize) {
  auto scriptBuffer = ScriptBuffers[scriptBufferId];
  ScriptBufferId = static_cast<uint8_t>(scriptBufferId);
  // String of characters by which tips are sorted, taken from _system script
  // auto sortString = (uint16_t *)ScriptGetTextTableStrAddress(2, 5);

  int idx = 0;

  // Read tips data from the script and create UI elements for each tip
  MemoryStream *stream =
      new MemoryStream(&scriptBuffer[tipsDataAdr], tipsDataSize);
  auto unk01 = ReadLE<uint16_t>(stream);
  while (unk01 != 255) {
    // Read tip entry from the data array
    TipsDataRecord record{.IsLocked = false, .IsUnread = false, .IsNew = false};
    record.Id = (uint16_t)idx;
    // I don't know, I don't care, this is not my magic
    record.SortLetterIndex = (unk01 - 5 * ((unk01 + 1) / 10) - 6);
    record.ThumbnailIndex = ReadLE<uint16_t>(stream);
    record.NumberOfContentStrings = ReadLE<uint16_t>(stream);
    for (int i = 0; i < record.NumberOfContentStrings + 3; i++) {
      record.StringAdr[i] =
          ScriptGetStrAddress(scriptBufferId, ReadLE<uint16_t>(stream));
    }
    Records[idx] = std::move(record);

    // Next tip entry from the data array
    unk01 = Io::ReadLE<uint16_t>(stream);
    idx += 1;
    TipEntryCount = idx;
  }
  Records.resize(TipEntryCount);

  UI::TipsMenuPtr->Init();

  delete stream;
}

void TipsSystem::UpdateTipRecords() {
  if (TipEntryCount != 0) {
    for (size_t i = 0; i < TipEntryCount; i++) {
      auto record = &Records[i];
      auto tipStatus = SaveSystem::GetTipStatus(record->Id);
      record->IsLocked = (tipStatus & 1) == 0;
      record->IsUnread = (tipStatus & 2) == 0;
      record->IsNew = (tipStatus & 4) == 0;
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

}  // namespace MO6TW
}  // namespace Impacto