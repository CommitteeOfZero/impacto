#pragma once

#include <flat_hash_map.hpp>
#include <unordered_map>
#include <map>
#include <vector>
#include "loadable.h"
#include "io/inputstream.h"

namespace Impacto {
struct VoiceMeta {
  uint16_t dataIndex;
  uint16_t voiceLengthSecTimes6;
};

class VoiceTable : public Loadable<VoiceTable> {
  friend class Loadable<VoiceTable>;

 public:
  uint8_t getVoiceData(uint32_t id, size_t index) {
    return lipSyncData[tableOfContents[id].dataIndex * 4 + index];
  }

 protected:
  bool LoadSync(uint32_t id);
  void UnloadSync();
  void MainThreadOnLoad();

 private:
  int voiceFileCount = 0;
  std::map<int, VoiceMeta> tableOfContents;
  std::vector<uint8_t> lipSyncData;
};

extern VoiceTable VoiceTableData;
}  // namespace Impacto