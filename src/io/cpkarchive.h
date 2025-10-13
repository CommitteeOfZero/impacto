#pragma once

#include "vfsarchive.h"
#include "memorystream.h"
#include <vector>
#include <variant>
#include <ankerl/unordered_dense.h>

namespace Impacto {
namespace Io {

struct CpkMetaEntry;

using CpkCell = std::variant<std::monostate, uint8_t, int8_t, uint16_t, int16_t,
                             uint32_t, int32_t, uint64_t, int64_t, float,
                             double, std::vector<uint8_t>, std::string>;

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

  bool ReadUtfBlock(
      std::vector<uint8_t>& utfBlock,
      std::vector<ankerl::unordered_dense::map<
          std::string, CpkCell, string_hash, std::equal_to<>>>& rows);
  std::string ReadString(int64_t stringsOffset);

  uint16_t Version;
  uint16_t Revision;

  MemoryStream* UtfStream = 0;
  CpkMetaEntry* FileList = 0;
  uint32_t FileCount = 0;
  uint32_t NextFile = 0;
};

}  // namespace Io
}  // namespace Impacto