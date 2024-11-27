#include "memorymappedfilestream.h"
#include "../log.h"

#include <cstring>
#include <algorithm>
#include <system_error>

namespace Impacto {
namespace Io {
template <AccessMode M>
IoError MemoryMappedFileStream<M>::Create(std::string const& fileName,
                                          Stream** out) {
  MemoryMappedFileStream* result =
      new MemoryMappedFileStream(GetSystemDependentPath(fileName));
  result->Meta.Size = GetFileSize(result->SourceFileName);
  if (result->Meta.Size == IoError_Fail) {
    delete result;
    return IoError_Fail;
  }

  std::error_code ec;
  result->mmapFile.map(result->SourceFileName, 0, result->Meta.Size, ec);
  if (ec) {
    ImpLog(LL_Error, LC_IO, "Failed to open file \"%s\", error:\"%s\"\n",
           result->SourceFileName.c_str(), ec.message().c_str());
    delete result;
    return ec == std::errc::no_such_file_or_directory ? IoError_Eof
                                                      : IoError_Fail;
  }
  *out = (Stream*)result;
  return IoError_OK;
}

template <AccessMode M>
int64_t MemoryMappedFileStream<M>::Read(void* buffer, int64_t sz) {
  if (sz < 0) return IoError_Fail;
  if (Position >= Meta.Size) return IoError_Eof;

  int bytesToRead = std::min(sz, Meta.Size - Position);
  assert(mmapFile.data());
  // Lock only if we are in read/write mode
  if constexpr (M == AccessMode::write) {
    std::shared_lock lock(*mutexLock);  // Lock for shared read access
    memcpy(buffer, mmapFile.data() + Position, bytesToRead);
  } else {
    memcpy(buffer, mmapFile.data() + Position, bytesToRead);
  }

  Position += bytesToRead;
  return bytesToRead;
}

template <AccessMode M>
int64_t MemoryMappedFileStream<M>::Seek(int64_t offset, int origin) {
  int64_t absPos;
  switch (origin) {
    case RW_SEEK_SET:
      absPos = offset;
      break;
    case RW_SEEK_CUR:
      absPos = Position + offset;
      break;
    case RW_SEEK_END:
      absPos = Meta.Size - offset;
      break;
  }

  if (absPos < 0 || absPos > Meta.Size) return IoError_Fail;
  Position = absPos;
  return Position;
}

template <AccessMode M>
IoError MemoryMappedFileStream<M>::Duplicate(Stream** outStream) {
  MemoryMappedFileStream* result = new MemoryMappedFileStream(*this);
  if (result->Seek(Position, RW_SEEK_SET) < 0) {
    ImpLog(LL_Error, LC_IO, "Seek failed for file \"%s\"\n",
           SourceFileName.c_str());
    delete result;
    return IoError_Fail;
  }
  *outStream = (Stream*)result;
  return IoError_OK;
}

template <AccessMode M>
int64_t MemoryMappedFileStream<M>::Write(void* buffer, int64_t sz, int cnt) {
  if constexpr (M == AccessMode::read) {
    assert(false);
    return 0;
  } else {
    std::lock_guard<std::shared_mutex> lock(*mutexLock);
    sz = std::min(Meta.Size - Position, sz);
    memcpy(mmapFile.data() + Position, buffer, sz);
    Position += sz;
    return sz;
  }
}

template class MemoryMappedFileStream<AccessMode::write>;
template class MemoryMappedFileStream<AccessMode::read>;

}  // namespace Io
}  // namespace Impacto