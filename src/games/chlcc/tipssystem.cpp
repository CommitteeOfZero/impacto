#include "tipssystem.h"

#include "../../data/savesystem.h"
#include "../../vm/vm.h"
#include "../../io/memorystream.h"
#include "../../profile/data/tipssystem.h"
#include "../../profile/games/chlcc/tipsmenu.h"
#include "../../profile/charset.h"
#include "../../ui/ui.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::Vm;
using namespace Impacto::Profile::TipsSystem;
using namespace Impacto::Profile::CHLCC::TipsMenu;
using namespace Impacto::Io;

void TipsSystem::DataInit(uint32_t scriptBufferId, uint32_t tipsDataAdr,
                          uint32_t tipsDataSize) {
  ScriptBufferId = (uint8_t)scriptBufferId;
  const auto scriptBuffer = ScriptBuffers[scriptBufferId];

  const std::unordered_map<uint16_t, int> strIndicesMap = [&] {
    std::unordered_map<uint16_t, int> sc3Map;
    auto [scrBufId, offset] =
        ScriptGetTextTableStrAddress(TipsStringTable, SortStringIndex);

    auto sortStr = &ScriptBuffers[scrBufId][offset];
    size_t i = 0;
    int distance = 0;
    while (sortStr[i] != 0xFF) {
      if (sortStr[i] & 0x80) {
        uint16_t currentSc3Char =
            SDL_SwapBE16(UnalignedRead<uint16_t>(sortStr + i));
        i += 2;
        sc3Map.try_emplace(currentSc3Char, distance++);
      } else {
        ImpLogSlow(LogLevel::Error, LogChannel::VM,
                   "TipsSorter: SC3 Tag Found in Sort String\n", sortStr[i]);
        i++;
      }
    }
    return sc3Map;
  }();

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
        .NumberOfContentStrings = numberOfContentStrings,
        .IsLocked = true,
        .IsUnread = true,
        .IsNew = true,
    };
    ReadLE<uint16_t>(&stream);  // Reads in a padding space string
    for (uint16_t i = 0; i < record.NumberOfContentStrings + 3; i++) {
      record.StringAdr[i] =
          ScriptGetStrAddress(scriptBufferId, ReadLE<uint16_t>(&stream));
    }
    auto sortStrIndex = record.StringAdr[2];
    auto firstChar = SDL_SwapBE16(
        UnalignedRead<uint16_t>(&ScriptBuffers[scriptBufferId][sortStrIndex]));
    auto sortIndexItr = strIndicesMap.find(firstChar);
    assert(sortIndexItr != strIndicesMap.end());
    record.CategoryLetterIndex =
        static_cast<uint16_t>(SortCategoryMapping->at(sortIndexItr->second));
    Records[TipEntryCount] = std::move(record);

    // Next tip entry from the data array
    numberOfContentStrings = Io::ReadLE<uint16_t>(&stream);
    TipEntryCount++;
  }

  Records.resize(TipEntryCount);
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

}  // namespace CHLCC
}  // namespace Impacto