#pragma once

#include "vfsarchive.h"
#include "memorystream.h"
#include <vector>
#include <flat_hash_map.hpp>

namespace Impacto {
namespace Io {

int const CpkMaxPath = 224;

struct CpkMetaEntry;

struct CpkCell {
  union {
    uint8_t Uint8Val;
    uint16_t Uint16Val;
    uint32_t Uint32Val;
    uint64_t Uint64Val;
    uint8_t* DataArray;
    float FloatVal;
    char StringVal[CpkMaxPath];
  };
  uint64_t DataSize;
};

class CpkArchive : public VfsArchive {
 public:
  ~CpkArchive();

  IoError Open(FileMeta* file, InputStream** outStream) override;
  IoError Slurp(FileMeta* file, void** outBuffer, int64_t* outSize) override;

  static IoError Create(InputStream* stream, VfsArchive** outArchive);

 private:
  IoError ReadToc(int64_t tocOffset, int64_t contentOffset);
  IoError ReadEtoc(int64_t etocOffset);
  IoError ReadItoc(int64_t itocOffset, int64_t contentOffset, uint16_t align);

  CpkMetaEntry* GetFileListEntry(uint32_t id);

  bool ReadUtfBlock(uint8_t* utfBlock,
                    std::vector<ska::flat_hash_map<std::string, CpkCell>>* rows,
                    uint64_t utfSize);
  void ReadString(int64_t stringsOffset, char* output);

  uint16_t Version;
  uint16_t Revision;

  MemoryStream* UtfStream = 0;
  CpkMetaEntry* FileList = 0;
  uint32_t FileCount = 0;
  uint32_t NextFile = 0;
};

}  // namespace Io
}  // namespace Impacto