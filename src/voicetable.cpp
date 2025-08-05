#include "voicetable.h"

#include "io/io.h"
#include "io/vfs.h"
#include "log.h"

namespace Impacto {
bool VoiceTable::LoadSync(uint32_t id) {
  Io::Stream* stream;
  int64_t err = Io::VfsOpen("system", id, &stream);
  if (err != IoError_OK) return false;
  VoiceFileCount = Io::ReadBE<uint16_t>(stream);
  Io::ReadBE<uint16_t>(stream);
  for (size_t i = 0; i < VoiceFileCount; ++i) {
    uint16_t dataIndex = Io::ReadBE<uint16_t>(stream);
    uint16_t voiceLengthSecTimes6 = Io::ReadLE<uint16_t>(stream);
    VoiceMeta voiceMeta{dataIndex, voiceLengthSecTimes6};
    TableOfContents[i] = voiceMeta;
  }
  [[maybe_unused]] const size_t endToc = 4 * VoiceFileCount + 4;
  assert((size_t)stream->Position == endToc);
  LipSyncData.resize(stream->Meta.Size - stream->Position);
  stream->Read(LipSyncData.data(), LipSyncData.size());
  delete stream;
  return true;
}

void VoiceTable::UnloadSync() {
  VoiceFileCount = 0;
  TableOfContents.clear();
  LipSyncData.resize(0);
}

void VoiceTable::MainThreadOnLoad(bool result) {}

}  // namespace Impacto