#pragma once

#include "stream.h"
#include <fstream>
#include <cstddef>
#include <shared_mutex>
#include <variant>
#include <mio/mio.hpp>

namespace Impacto {
namespace Io {

using AccessMode = mio::access_mode;

template <AccessMode Access>
class MemoryMappedFileStream : public Stream {
 public:
  static constexpr AccessMode Mode = Access;

  static IoError Create(std::string const& fileName, Stream** out);
  int64_t Read(void* buffer, int64_t sz) override;
  int64_t Seek(int64_t offset, int origin) override;
  IoError Duplicate(Stream** outStream) override;
  int64_t Write(const void* buffer, int64_t sz, size_t cnt = 1) override;

 protected:
  MemoryMappedFileStream(std::string filePath)
      : SourceFileName(std::move(filePath)) {
    Meta.FileName = SourceFileName;
    if constexpr (Access == AccessMode::write) {
      mutexLock = std::make_shared<std::shared_mutex>();
    }
  }

  MemoryMappedFileStream(MemoryMappedFileStream const& other)
      : SourceFileName(other.SourceFileName), mmapFile(other.mmapFile) {
    Meta.FileName = SourceFileName;
    Meta.Size = other.Meta.Size;
    if constexpr (Access == AccessMode::write) {
      mutexLock = other.mutexLock;
    }
  }
  std::string SourceFileName;
  mio::basic_shared_mmap<Access, std::byte> mmapFile;

  using WriteLockMember =
      std::conditional_t<Access == AccessMode::write,
                         std::shared_ptr<std::shared_mutex>, std::monostate>;
  WriteLockMember mutexLock;
};
}  // namespace Io
}  // namespace Impacto