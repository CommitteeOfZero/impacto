#include "physicalfilestream.h"
#include "../log.h"

#include <cstring>
#include <algorithm>
#include <system_error>

namespace Impacto {
namespace Io {
std::ios_base::openmode PhysicalFileStream::PrepareFileOpenMode(
    CreateFlags flags, std::error_code& ec) {
  std::ios_base::openmode mode = std::ios::binary;
  bool fileExists = std::filesystem::exists(SourceFileName, ec);
  if (ec) {
    ImpLog(LL_Error, LC_IO,
           "Failed to check whether file exists \"%s\", error: \"%s\"\n",
           SourceFileName.generic_string().c_str(), ec.message().c_str());
    return {};
  }

  bool writeNoExistNoCreate =
      (flags & WRITE) && !fileExists && !(flags & CREATE_IF_NOT_EXISTS);
  bool readNoExist = !(flags & WRITE) && (flags & READ) && !fileExists;
  if (writeNoExistNoCreate || readNoExist) {
    ec = std::make_error_code(std::errc::no_such_file_or_directory);
    ImpLog(LL_Error, LC_IO, "Failed to open stream \"%s\", error: \"%s\"\n",
           SourceFileName.generic_string().c_str(), ec.message().c_str());
    return {};
  }

  bool writeExistNoOverwrite =
      (flags & WRITE) && fileExists && !(flags & TRUNCATE) && !(flags & APPEND);
  if (writeExistNoOverwrite) {
    // avoid truncating when in write only mode without truncate flag to
    // preserve file size
    // I think this is more intuitive than making it an error
    // or letting the truncate happen
    flags |= CreateFlagsMode::READ;
  }

  // require write for create/overwrite flags
  assert((flags & WRITE) || !(flags & CREATE_DIRS));
  assert((flags & WRITE) || !(flags & CREATE_IF_NOT_EXISTS));

  // truncate is only needed when creating nonexistent if also reading
  bool truncFlag = (flags & TRUNCATE) ||
                   ((flags & READ) && !fileExists &&
                    (flags & CREATE_IF_NOT_EXISTS) && !(flags & APPEND));

  // trunc and append are mutually exclusive
  assert(!truncFlag || !(flags & APPEND));

  if (flags & READ) mode |= std::ios::in;
  if (flags & WRITE) mode |= std::ios::out;
  if (flags & APPEND) mode |= std::ios::app;
  if (truncFlag) mode |= std::ios::trunc;
  if (flags & CREATE_DIRS) {
    std::filesystem::create_directories(SourceFileName.parent_path(), ec);
    if (ec) {
      ImpLog(LL_Error, LC_IO,
             "Failed to create directories for file \"%s\", error: \"%s\"\n",
             SourceFileName.generic_string().c_str(), ec.message().c_str());
      return {};
    }
  }
  return mode;
}

IoError PhysicalFileStream::Create(std::string const& fileName, Stream** out,
                                   CreateFlags flags) {
  std::error_code ec;
  PhysicalFileStream* result = new PhysicalFileStream(fileName, flags);
  if (result->ErrorCode) {
    ImpLog(LL_Error, LC_IO, "Failed to open file \"%s\", error: \"%s\"\n",
           fileName.c_str(), result->ErrorCode.message().c_str());
    delete result;
    return result->ErrorCode == std::errc::no_such_file_or_directory
               ? IoError_NotFound
               : IoError_Fail;
  }
  if (!result->FileStream) {
    ImpLog(LL_Error, LC_IO, "Failed to open file \"%s\", error: \"%s\"\n",
           fileName.c_str(), std::generic_category().message(errno).c_str());
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
  if (sz < 0 || !(Flags & READ)) return IoError_Fail;
  if (Position >= Meta.Size || FileStream.eof()) {
    FileStream.clear(FileStream.rdstate() & ~std::ios::failbit &
                     ~std::ios::eofbit);  // Clear only failbit and eofbit
    return IoError_Eof;
  };
  int bytesToRead = std::min(sz, Meta.Size - Position);
  FileStream.read((char*)buffer, bytesToRead);
  if (!FileStream) {
    ImpLog(LL_Error, LC_IO, "Read failed for file \"%s\" with error: \"%s\"\n",
           SourceFileName.string().c_str(),
           std::generic_category().message(errno).c_str());
    FileStream.clear(FileStream.rdstate() & ~std::ios::failbit &
                     ~std::ios::eofbit);  // Clear only failbit and eofbit
    return IoError_Fail;
  }
  auto read = FileStream.gcount();
  Position += read;
  return read;
}

int64_t PhysicalFileStream::Seek(int64_t offset, int origin) {
  if (!(Flags & READ) && (Flags & APPEND)) {
    // seeking is useless in readless append mode
    return IoError_Fail;
  }
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

  // seeking past EOF is a legal operation, after write past EOF, the gap
  // between prev file size and write pos is zero padded
  if (absPos < 0) return IoError_Fail;
  FileStream.seekg(absPos, std::ios::beg);
  if (!FileStream && !FileStream.eof()) {
    ImpLog(LL_Error, LC_IO, "Seek failed for file \"%s\" with error: \"%s\"\n",
           SourceFileName.string().c_str(),
           std::generic_category().message(errno).c_str());
    FileStream.clear(FileStream.rdstate() & ~std::ios::failbit &
                     ~std::ios::eofbit);  // Clear only failbit and eofbit
    return IoError_Fail;
  }
  Position = FileStream.tellg();
  return Position;
}

IoError PhysicalFileStream::Duplicate(Stream** outStream) {
  PhysicalFileStream* result = new PhysicalFileStream(*this);
  std::error_code ec;
  if (result->ErrorCode) {
    ImpLog(LL_Error, LC_IO, "Failed to open file \"%s\", error: \"%s\"\n",
           SourceFileName.string().c_str(),
           result->ErrorCode.message().c_str());
    delete result;
    return result->ErrorCode == std::errc::no_such_file_or_directory
               ? IoError_NotFound
               : IoError_Fail;
  }
  if (!result->FileStream) {
    ImpLog(LL_Error, LC_IO, "Failed to open file \"%s\", error: \"%s\"\n",
           SourceFileName.string().c_str(),
           std::generic_category().message(errno).c_str());
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
           SourceFileName.string().c_str(),
           std::generic_category().message(errno).c_str());
    delete result;
    return IoError_Fail;
  }
  *outStream = (Stream*)result;
  return IoError_OK;
}

int64_t PhysicalFileStream::Write(void* buffer, int64_t sz, int cnt) {
  if (!(Flags & WRITE)) {
    return IoError_Fail;
  }
  FileStream.write((char*)buffer, sz * cnt);
  if (!FileStream) {
    ImpLog(LL_Error, LC_IO, "Write failed for file \"%s\" with error: \"%s\"\n",
           SourceFileName.string().c_str(),
           std::generic_category().message(errno).c_str());
    FileStream.clear(FileStream.rdstate() & ~std::ios::failbit &
                     ~std::ios::eofbit);  // Clear only failbit and eofbit
    return IoError_Fail;
  }
  int64_t written = sz * cnt;
  Position += written;
  Meta.Size = std::max(Position, Meta.Size);
  return written;
}

}  // namespace Io
}  // namespace Impacto