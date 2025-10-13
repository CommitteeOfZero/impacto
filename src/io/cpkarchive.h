#pragma once

#include "vfsarchive.h"
#include "memorystream.h"
#include <vector>
#include <ankerl/unordered_dense.h>

namespace Impacto {
namespace Io {

struct CpkMetaEntry;

class CpkArchive : public VfsArchive {
 public:
  ~CpkArchive();

  IoError Open(FileMeta* file, Stream** outStream) override;
  IoError Slurp(FileMeta* file, void*& outBuffer, int64_t& outSize) override;

  static IoError Create(Stream* stream, VfsArchive** outArchive);

 private:
  IoError ReadToc(int64_t tocOffset, int64_t contentOffset);
  IoError ReadEtoc(int64_t etocOffset);
  IoError ReadItoc(int64_t itocOffset, int64_t contentOffset, uint16_t align);

  CpkMetaEntry* GetFileListEntry(uint32_t id);

  uint16_t Version;
  uint16_t Revision;

  CpkMetaEntry* FileList = 0;
  uint32_t FileCount = 0;
  uint32_t NextFile = 0;
};

}  // namespace Io
}  // namespace Impacto