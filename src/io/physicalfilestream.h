#pragma once

#include "stream.h"
#include <fstream>

namespace Impacto {
namespace Io {

class PhysicalFileStream : public Stream {
 public:
  enum CreateFlagsMode {
    READ = 1,
    WRITE = READ << 1,
    CREATE_IF_NOT_EXISTS = WRITE << 1,
    TRUNCATE = CREATE_IF_NOT_EXISTS << 1,
    APPEND = TRUNCATE << 1,
    CREATE_DIRS = APPEND << 1,
    UNBUFFERED = CREATE_DIRS << 1
  };
  using CreateFlags = int;
  static IoError Create(std::string const& fileName, Stream** out,
                        CreateFlags flags = CreateFlagsMode::READ);
  int64_t Read(void* buffer, int64_t sz) override;
  int64_t Seek(int64_t offset, int origin) override;
  IoError Duplicate(Stream** outStream) override;
  int64_t Write(void* buffer, int64_t sz, size_t cnt = 1) override;

 protected:
  std::ios_base::openmode PrepareFileOpenMode(CreateFlags flags);

  PhysicalFileStream(std::string filePath, CreateFlags flags)
      : Flags(flags),
        SourceFileName(std::move(filePath)),
        FileStream(SourceFileName, PrepareFileOpenMode(flags)) {
    if (flags & UNBUFFERED) {
      FileStream.rdbuf()->pubsetbuf(nullptr, 0);
    }
    Meta.FileName = SourceFileName;
  }

  PhysicalFileStream(std::string filePath)
      : PhysicalFileStream(std::move(filePath), CreateFlagsMode::READ) {}

  PhysicalFileStream(PhysicalFileStream const& other)
      : Flags(other.Flags),
        SourceFileName(other.SourceFileName),
        FileStream(other.SourceFileName, PrepareFileOpenMode(Flags)) {
    if (Flags & UNBUFFERED) {
      FileStream.rdbuf()->pubsetbuf(nullptr, 0);
    }
    Meta.FileName = SourceFileName;
  }
  IoError ErrorCode = IoError_OK;
  CreateFlags Flags;
  std::string SourceFileName;
  std::fstream FileStream;
};

}  // namespace Io
}  // namespace Impacto