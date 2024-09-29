#include "physicalfilestream.h"
#include "../log.h"

#include <cstring>
#include <algorithm>

namespace Impacto {
namespace Io {

IoError PhysicalFileStream::Create(std::string const& fileName, Stream** out,
                                   bool exists) {
  std::error_code ec;
  if (exists && !std::filesystem::exists(fileName, ec)) {
    if (ec) {
      ImpLog(LL_Error, LC_IO, "Error checking file existence: %s\n",
             ec.message().c_str());
      return IoError_Fail;
    }
    return IoError_NotFound;
  }
  PhysicalFileStream* result = new PhysicalFileStream(fileName, !exists);
  if (!result->FileStream) {
    ImpLog(LL_Error, LC_IO, "Failed to open file \"%s\", error: \"%s\"\n",
           fileName.c_str(), std::strerror(errno));
    delete result;
    return IoError_Fail;
  }
  result->Meta.Size = std::filesystem::file_size(result->SourceFileName, ec);
  if (ec) {
    ImpLog(LL_Error, LC_IO, "Error getting file size: %s\n",
           ec.message().c_str());
    delete result;
    return IoError_Fail;
  }
  *out = (Stream*)result;
  return IoError_OK;
}

int64_t PhysicalFileStream::Read(void* buffer, int64_t sz) {
  if (FileStream.eof()) {
    return IoError_Eof;
  }
  FileStream.read((char*)buffer, sz);
  if (!FileStream && !FileStream.eof()) {
    ImpLog(LL_Error, LC_IO, "Read failed for file \"%s\" with error: \"%s\"\n",
           SourceFileName.string().c_str(), std::strerror(errno));
    return IoError_Fail;
  }
  int64_t read = FileStream.gcount();
  Position += read;
  return read;
}

int64_t PhysicalFileStream::Seek(int64_t offset, int origin) {
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
  FileStream.seekg(absPos, std::ios::beg);
  if (!FileStream && !FileStream.eof()) {
    ImpLog(LL_Error, LC_IO, "Seek failed for file \"%s\" with error: \"%s\"\n",
           SourceFileName.string().c_str(), std::strerror(errno));
    return IoError_Fail;
  }
  Position = FileStream.tellg();
  return Position;
}

IoError PhysicalFileStream::Duplicate(Stream** outStream) {
  PhysicalFileStream* result = new PhysicalFileStream(*this);
  std::error_code ec;
  if (!result->FileStream) {
    ImpLog(LL_Error, LC_IO, "Failed to open file \"%s\", error: \"%s\"\n",
           SourceFileName.string().c_str(), std::strerror(errno));
    delete result;
    return IoError_Fail;
  }
  result->Meta.Size = std::filesystem::file_size(SourceFileName, ec);
  if (ec) {
    ImpLog(LL_Error, LC_IO, "Error getting file size: %s\n",
           ec.message().c_str());
    delete result;
    return IoError_Fail;
  }
  if (result->Seek(Position, RW_SEEK_SET) < 0) {
    ImpLog(LL_Error, LC_IO, "Seek failed for file \"%s\" with error: \"%s\"\n",
           SourceFileName.string().c_str(), std::strerror(errno));
    delete result;
    return IoError_Fail;
  }
  *outStream = (Stream*)result;
  return IoError_OK;
}

int64_t PhysicalFileStream::Write(void* buffer, int64_t sz, int cnt) {
  FileStream.write((char*)buffer, sz * cnt);
  if (!FileStream) {
    ImpLog(LL_Error, LC_IO, "Write failed for file \"%s\" with error: \"%s\"\n",
           SourceFileName.string().c_str(), std::strerror(errno));
    return IoError_Fail;
  }
  int64_t written = sz * cnt;
  Position += written;
  Meta.Size = std::max(Position, Meta.Size);
  return written;
}

}  // namespace Io
}  // namespace Impacto