#pragma once

#include "stream.h"
#include <fstream>
#include <filesystem>
#include <system_error>
#include "buffering.h"

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
    CREATE_DIRS = CREATE_IF_NOT_EXISTS << 1
  };
  using CreateFlags = int;
  static IoError Create(std::string const& fileName, Stream** out,
                        CreateFlags flags = CreateFlagsMode::READ);
  int64_t Read(void* buffer, int64_t sz) override;
  int64_t Seek(int64_t offset, int origin) override;
  IoError Duplicate(Stream** outStream) override;
  int64_t Write(void* buffer, int64_t sz, int cnt = 1) override;

 protected:
  std::ios_base::openmode PrepareFileOpenMode(CreateFlags flags,
                                              std::error_code& ec);

  PhysicalFileStream(std::filesystem::path filePath, CreateFlags flags)
      : Flags(flags),
        SourceFileName(std::move(filePath)),
        FileStream(SourceFileName, PrepareFileOpenMode(flags, ErrorCode)) {
    Meta.FileName = SourceFileName.string();
  }

  PhysicalFileStream(std::filesystem::path filePath)
      : PhysicalFileStream(std::move(filePath), CreateFlagsMode::READ) {}

  PhysicalFileStream(PhysicalFileStream const& other)
      : Flags(other.Flags),
        SourceFileName(other.SourceFileName),
        FileStream(other.SourceFileName,
                   PrepareFileOpenMode(Flags, ErrorCode)) {
    Meta.FileName = SourceFileName.string();
  }
  std::error_code ErrorCode;
  CreateFlags Flags;
  std::filesystem::path SourceFileName;
  std::fstream FileStream;
};

}  // namespace Io
}  // namespace Impacto