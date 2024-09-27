#pragma once

#include "stream.h"
#include <fstream>
#include <filesystem>
#include "buffering.h"

namespace Impacto {
namespace Io {

class PhysicalFileStream : public Stream {
 public:
  static IoError Create(std::string const& fileName, Stream** out,
                        bool exists = true);
  int64_t Read(void* buffer, int64_t sz) override;
  int64_t Seek(int64_t offset, int origin) override;
  IoError Duplicate(Stream** outStream) override;
  int64_t Write(void* buffer, int64_t sz, int cnt = 1) override;

 protected:
  std::ios_base::openmode GetFileMode(bool truncate) {
    // trunc will clear file if it exists, and allows creation of new file
    return (truncate) ? std::ios::in | std::ios::out | std::ios::trunc |
                            std::ios::binary
                      : std::ios::in | std::ios::out | std::ios::binary;
  }
  PhysicalFileStream(std::filesystem::path filePath, bool truncate = false)
      : Truncate(truncate),
        SourceFileName(std::move(filePath)),
        FileStream(SourceFileName, GetFileMode(truncate)) {
    Meta.FileName = SourceFileName.string();
  }

  PhysicalFileStream(PhysicalFileStream const& other)
      : SourceFileName(other.SourceFileName),
        FileStream(other.SourceFileName, GetFileMode(other.Truncate)) {
    Meta.FileName = SourceFileName.string();
  }
  bool Truncate;
  std::filesystem::path SourceFileName;
  std::fstream FileStream;
};

}  // namespace Io
}  // namespace Impacto