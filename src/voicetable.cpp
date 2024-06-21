#include "voicetable.h"

#include "io/io.h"
#include "io/vfs.h"
#include "log.h"

namespace Impacto {
bool VoiceTable::LoadSync(uint32_t id) {
  Io::Stream* stream;
  int64_t err = Io::VfsOpen("system", id, &stream);
  if (err != IoError_OK) return false;
  uint16_t voiceFileCount = Io::ReadBE<uint16_t>(stream);
  Io::ReadBE<uint16_t>(stream);
  for (size_t i = 0; i < voiceFileCount; ++i) {
    uint16_t dataIndex = Io::ReadBE<uint16_t>(stream);
    uint16_t voiceLengthSecTimes6 = Io::ReadLE<uint16_t>(stream);
    VoiceMeta voiceMeta{dataIndex, voiceLengthSecTimes6};
    tableOfContents[i] = voiceMeta;
  }
  const size_t endToc = 4 * voiceFileCount + 4;
  assert(stream->Position == endToc);
  lipSyncData.resize(stream->Meta.Size - stream->Position);
  stream->Read(lipSyncData.data(), lipSyncData.size());
  delete stream;
  return true;
}

void VoiceTable::UnloadSync() {
  voiceFileCount = 0;
  tableOfContents.clear();
  lipSyncData.resize(0);
}

void VoiceTable::MainThreadOnLoad() {}

}  // namespace Impacto