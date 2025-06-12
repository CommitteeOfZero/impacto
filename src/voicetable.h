#pragma once

#include <ankerl/unordered_dense.h>
#include <unordered_map>
#include <map>
#include <vector>
#include "loadable.h"
#include "io/stream.h"

namespace Impacto {
struct VoiceMeta {
  uint16_t DataIndex;
  uint16_t VoiceLengthSecTimes6;
};

class VoiceTable : public Loadable<VoiceTable, bool, uint32_t> {
  friend class Loadable<VoiceTable, bool, uint32_t>;

 public:
  uint8_t GetVoiceData(uint32_t id, size_t index) {
    return LipSyncData[TableOfContents[id].DataIndex * 4 + index];
  }

 protected:
  bool LoadSync(uint32_t id);
  void UnloadSync();
  void MainThreadOnLoad(bool result);

 private:
  int VoiceFileCount = 0;
  std::map<size_t, VoiceMeta> TableOfContents;
  std::vector<uint8_t> LipSyncData;
};

inline VoiceTable VoiceTableData;
}  // namespace Impacto