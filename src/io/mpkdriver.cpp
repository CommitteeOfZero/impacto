#include <inttypes.h>

#include <zlib.h>

#include "mpkdriver.h"
#include "vfs.h"
#include "../log.h"

#include "../../vendor/include/flat_hash_map.hpp"

namespace Impacto {

const int MpkMaxPath = 224;

const int MpkZlibBufferSize = 256 * 1024;
const int MpkZlibSeekBufferSize = 16 * 1024;

struct MpkMetaEntry {
  uint32_t Id;
  char Name[MpkMaxPath];
  int64_t CompressedSize;
  int64_t UncompressedSize;
  int64_t Offset;
  bool Compressed;
};

class MpkArchive : public VfsArchive {
 public:
  uint16_t MinorVersion;
  uint16_t MajorVersion;

  uint32_t FileCount;
  MpkMetaEntry* TOC;

  ska::flat_hash_map<uint32_t, uint32_t> IdsToFiles;
  ska::flat_hash_map<std::string, uint32_t> NamesToIds;

  IoError GetName(uint32_t id, char* outName) override;
  IoError GetId(const char* path, uint32_t* outId) override;

  IoError EnumerateStart(uint32_t* outIterator,
                         VfsFileInfo* outFileInfo) override;
  IoError EnumerateNext(uint32_t* inoutIterator,
                        VfsFileInfo* outFileInfo) override;

  ~MpkArchive() override;

 protected:
  IoError DriverOpen(uint32_t id, SDL_RWops** outHandle) override;
  IoError DriverGetSize(uint32_t id, int64_t* outSize) override;
  IoError DriverCanStream(uint32_t id, bool* outResult) override;
  IoError DriverSlurp(uint32_t id, void* outBuffer) override;
};

struct MpkOpenFile {
  MpkMetaEntry* Entry;
  MpkArchive* Archive;

  int64_t Position;
};

struct MpkOpenCompressedFile : MpkOpenFile {
  uint8_t Buffer[MpkZlibBufferSize];
  uint8_t SeekBuffer[MpkZlibSeekBufferSize];
  z_stream ZlibStream;
};

static bool registered = false;

int64_t SDLCALL MpkEntrySize(SDL_RWops* context) {
  MpkOpenFile* file = (MpkOpenFile*)(context->hidden.unknown.data1);
  return file->Entry->UncompressedSize;
}

size_t SDLCALL MpkEntryUncompressedRead(SDL_RWops* context, void* ptr,
                                        size_t size, size_t maxnum) {
  int64_t bytes = size * maxnum;
  MpkOpenFile* file = (MpkOpenFile*)context->hidden.unknown.data1;
  int64_t fileCur = file->Entry->Offset + file->Position;
  int64_t fileEnd = file->Entry->Offset + file->Entry->UncompressedSize;
  int64_t availBytes = std::min(fileEnd - fileCur, bytes);
  int64_t availNum = availBytes / size;
  if (SDL_RWseek(file->Archive->BaseStream, fileCur, RW_SEEK_SET) == -1) {
    ImpLog(LL_Error, LC_IO,
           "Reading %" PRIu64 " bytes at %" PRId64
           " from open archive \"%s\" failed (seek error): %s\n",
           bytes, fileCur, file->Archive->MountPoint, SDL_GetError());
    return 0;
  }
  ImpLogSlow(LL_Trace, LC_IO, "read size=%" PRId64 " num=%" PRId64 "\n", size,
             availNum);
  size_t result = SDL_RWread(file->Archive->BaseStream, ptr, size, availNum);
  file->Position += size * result;
  if (result == 0) {
    ImpLog(LL_Error, LC_IO,
           "Reading %" PRIu64 " bytes at %" PRId64
           " from open archive \"%s\" failed (read error): %s\n",
           bytes, fileCur, file->Archive->MountPoint, SDL_GetError());
  }
  return result;
}

size_t SDLCALL MpkEntryCompressedRead(SDL_RWops* context, void* ptr,
                                      size_t size, size_t maxnum) {
  int64_t bytes = size * maxnum;
  MpkOpenCompressedFile* file =
      (MpkOpenCompressedFile*)context->hidden.unknown.data1;
  int64_t fileCur = file->Entry->Offset + file->Position;
  int64_t fileEnd = file->Entry->Offset + file->Entry->CompressedSize;
  if (SDL_RWseek(file->Archive->BaseStream, fileCur, RW_SEEK_SET) == -1) {
    ImpLog(LL_Error, LC_IO,
           "Reading %" PRIu64 " decompressed bytes at %" PRId64
           " from open archive \"%s\" failed (seek error)\n",
           bytes, fileCur, file->Archive->MountPoint);
    return 0;
  }

  int64_t lastTotal = file->ZlibStream.total_out;
  file->ZlibStream.avail_out = bytes;
  file->ZlibStream.next_out = (Bytef*)ptr;

  int zErr;
  do {
    if (file->ZlibStream.avail_in == 0) {
      // If input buffer is empty, fill up to end of compressed file
      file->ZlibStream.next_in = file->Buffer;
      file->ZlibStream.avail_in =
          SDL_RWread(file->Archive->BaseStream, file->ZlibStream.next_in, 1,
                     std::min(fileEnd - fileCur, (int64_t)MpkZlibBufferSize));
      // and add to total consumed bytes
      file->Position += file->ZlibStream.avail_in;
      fileCur += file->ZlibStream.avail_in;
    }
    // If we didn't read anything, must be past end of file
    if (file->ZlibStream.avail_in == 0) {
      ImpLog(LL_Error, LC_IO,
             "Reading %" PRIu64 " decompressed bytes at %" PRId64
             " from open archive \"%s\" failed (end of compressed file)\n",
             bytes, fileCur, file->Archive->MountPoint);
      return (file->ZlibStream.total_out - lastTotal) / size;
    }

    zErr = inflate(&file->ZlibStream, Z_SYNC_FLUSH);
  } while (zErr == Z_OK && file->ZlibStream.avail_out > 0);

  if (zErr != Z_OK && zErr != Z_STREAM_END) {
    ImpLog(LL_Error, LC_IO,
           "Reading %" PRIu64 " decompressed bytes at %" PRId64
           " from open archive \"%s\" failed (inflate() error %d)\n",
           bytes, fileCur, file->Archive->MountPoint, zErr);
  }

  return (file->ZlibStream.total_out - lastTotal) / size;
}

int64_t SDLCALL MpkEntryUncompressedSeek(SDL_RWops* context, int64_t offset,
                                         int whence) {
  MpkOpenFile* file = (MpkOpenFile*)context->hidden.unknown.data1;
  int64_t relOffset;
  switch (whence) {
    case RW_SEEK_SET: {
      relOffset = offset - file->Position;
      break;
    }

    case RW_SEEK_CUR: {
      relOffset = offset;
      break;
    }

    case RW_SEEK_END: {
      int64_t invOffset =
          file->Entry->UncompressedSize - offset;  // equivalent for SEEK_SET
      relOffset = invOffset - file->Position;
      break;
    }

    default: {
      ImpLog(LL_Error, LC_IO,
             "Invalid seek type %d in file \"%s\" in archive \"%s\"\n", whence,
             file->Entry->Name, file->Archive->MountPoint);
      return SDL_SetError("Invalid seek type");
      break;
    }
  }
  if (file->Position + relOffset > file->Entry->UncompressedSize ||
      file->Position + relOffset < 0) {
    ImpLog(LL_Error, LC_IO,
           "Invalid seek (mode %d, offset %" PRId64
           ") in file \"%s\" in archive \"%s\"\n",
           whence, offset, file->Entry->Name, file->Archive->MountPoint);
    return SDL_SetError("Out-of-bounds seek");
  } else {
    file->Position += relOffset;
    return file->Position;
  }
}
int64_t SDLCALL MpkEntryCompressedSeek(SDL_RWops* context, int64_t offset,
                                       int whence) {
  MpkOpenCompressedFile* file =
      (MpkOpenCompressedFile*)context->hidden.unknown.data1;
  int64_t relOffset;
  int64_t currentPosition = file->ZlibStream.total_out;
  switch (whence) {
    case RW_SEEK_SET: {
      relOffset = offset - currentPosition;
      break;
    }

    case RW_SEEK_CUR: {
      relOffset = offset;
      break;
    }

    case RW_SEEK_END: {
      int64_t invOffset =
          file->Entry->UncompressedSize - offset;  // equivalent for SEEK_SET
      relOffset = invOffset - currentPosition;
      break;
    }

    default: {
      ImpLog(LL_Error, LC_IO,
             "Invalid seek type %d in file \"%s\" in archive \"%s\"\n", whence,
             file->Entry->Name, file->Archive->MountPoint);
      return SDL_SetError("Invalid seek type");
      break;
    }
  }
  if (currentPosition + relOffset > file->Entry->UncompressedSize ||
      currentPosition + relOffset < 0) {
    ImpLog(LL_Error, LC_IO,
           "Invalid seek (mode %d, offset %" PRId64
           ") in file \"%s\" in archive \"%s\"\n",
           whence, offset, file->Entry->Name, file->Archive->MountPoint);
    return SDL_SetError("Out-of-bounds seek");
  } else {
    if (relOffset < 0) {
      // rewind
      int64_t absOffset = currentPosition + relOffset;
      inflateReset(&file->ZlibStream);
      file->ZlibStream.avail_in = 0;
      file->ZlibStream.next_in = NULL;
      // then fast forward (fallthrough)
    }
    if (relOffset == 0) {
      return currentPosition;
    }
    if (relOffset > 0) {
      int64_t bytesRemaining = relOffset;
      int64_t bytesToRead;
      do {
        bytesToRead = std::min(bytesRemaining, (int64_t)MpkZlibSeekBufferSize);
      } while (MpkEntryCompressedRead(context, file->SeekBuffer, 1,
                                      bytesRemaining) > 0);
      return file->ZlibStream.total_out;
    }
  }

  return SDL_SetError("Fell to end of seek function");
}

size_t SDLCALL MpkEntryWrite(SDL_RWops* context, const void* ptr, size_t size,
                             size_t num) {
  const char error[] = "Tried to write to an MPK entry";
  SDL_SetError(error);
  ImpLog(LL_Error, LC_IO, "%s\n", error);
  return 0;
}

int SDLCALL MpkEntryClose(SDL_RWops* context) {
  MpkOpenFile* file = (MpkOpenFile*)(context->hidden.unknown.data1);
  ImpLog(LL_Debug, LC_IO, "Closing file \"%s\" in \"%s\"\n", file->Entry->Name,
         file->Archive->MountPoint);
  if (file->Archive->OpenHandles == 0) {
    ImpLog(LL_Error, LC_IO,
           "Closed file \"%s\" in \"%s\"\n while no file handles were "
           "open\n",
           file->Entry->Name, file->Archive->MountPoint);
  } else {
    file->Archive->OpenHandles--;
  }

  free(context);
  free(file);
  return 0;
}

int SDLCALL MpkEntryCompressedClose(SDL_RWops* context) {
  MpkOpenCompressedFile* file =
      (MpkOpenCompressedFile*)(context->hidden.unknown.data1);
  ImpLog(LL_Debug, LC_IO, "Closing compressed file \"%s\" in \"%s\"\n",
         file->Entry->Name, file->Archive->MountPoint);

  inflateEnd(&file->ZlibStream);

  return MpkEntryClose(context);
}

IoError MpkMount(SDL_RWops* stream, VfsArchive** outArchive) {
  const uint32_t magic = 0x4D504B00;
  if (SDL_ReadBE32(stream) != magic) {
    ImpLog(LL_Trace, LC_IO, "Not an MPK\n");
    return IoError_Fail;
  }

  uint16_t MinorVersion = SDL_ReadLE16(stream);
  uint16_t MajorVersion = SDL_ReadLE16(stream);
  // TODO support v1
  if (MinorVersion != 0 || MajorVersion != 2) {
    ImpLog(LL_Trace, LC_IO, "Unsupported MPK version %" PRIu16 ".%" PRIu16 "\n",
           MajorVersion, MinorVersion);
    return IoError_Fail;
  }

  MpkArchive* result = new MpkArchive;
  result->MinorVersion = MinorVersion;
  result->MajorVersion = MajorVersion;
  result->FileCount = SDL_ReadLE32(stream);
  result->TOC = (MpkMetaEntry*)calloc(result->FileCount, sizeof(MpkMetaEntry));
  result->NamesToIds.reserve(result->FileCount);
  result->IdsToFiles.reserve(result->FileCount);
  SDL_RWseek(stream, 0x40, RW_SEEK_SET);
  for (uint32_t i = 0; i < result->FileCount; i++) {
    MpkMetaEntry* entry = &result->TOC[i];
    uint32_t Compression = SDL_ReadLE32(stream);
    if (Compression != 1 && Compression != 0) {
      ImpLog(LL_Trace, LC_IO, "Unknown MPK compression type %d\n", Compression);
      delete result;
      return IoError_Fail;
    }
    entry->Compressed = Compression == 1;
    entry->Id = SDL_ReadLE32(stream);
    entry->Offset = SDL_ReadLE64(stream);
    entry->CompressedSize = SDL_ReadLE64(stream);
    entry->UncompressedSize = SDL_ReadLE64(stream);
    SDL_RWread(stream, entry->Name, MpkMaxPath, 1);

    result->IdsToFiles[entry->Id] = i;
    result->NamesToIds[entry->Name] = entry->Id;
  }

  ImpLog(LL_Debug, LC_IO, "Mounted as MPK\n");
  *outArchive = result;
  return IoError_OK;
}

MpkArchive::~MpkArchive() { free(TOC); }

IoError MpkArchive::DriverOpen(uint32_t id, SDL_RWops** outHandle) {
  auto it = IdsToFiles.find(id);
  if (it == IdsToFiles.end()) {
    ImpLog(LL_Error, LC_IO, "MPK open: %d not found in \"%s\"\n", id,
           MountPoint);
    return IoError_NotFound;
  }

  SDL_RWops* rwops = (SDL_RWops*)calloc(1, sizeof(SDL_RWops));
  rwops->type = SDL_RWOPS_UNKNOWN;
  rwops->size = MpkEntrySize;
  rwops->write = MpkEntryWrite;

  MpkMetaEntry* entry = &TOC[it->second];

  if (entry->Compressed) {
    rwops->close = MpkEntryCompressedClose;
    rwops->read = MpkEntryCompressedRead;
    rwops->seek = MpkEntryCompressedSeek;

    MpkOpenCompressedFile* file =
        (MpkOpenCompressedFile*)calloc(1, sizeof(MpkOpenCompressedFile));
    file->Archive = this;
    file->Entry = entry;
    file->Position = 0;

    int zErr;
    if ((zErr = inflateInit(&file->ZlibStream)) != Z_OK) {
      ImpLog(LL_Error, LC_IO,
             "MPK open: Zlib initialisation failed for file %d in \"%s\" with "
             "code %d\n",
             id, MountPoint, zErr);
      free(rwops);
      free(file);
      return IoError_Fail;
    }

    rwops->hidden.unknown.data1 = file;
  } else {
    rwops->close = MpkEntryClose;
    rwops->read = MpkEntryUncompressedRead;
    rwops->seek = MpkEntryUncompressedSeek;

    MpkOpenFile* file = (MpkOpenFile*)calloc(1, sizeof(MpkOpenFile));
    file->Archive = this;
    file->Entry = entry;
    file->Position = 0;

    rwops->hidden.unknown.data1 = file;
  }

  *outHandle = rwops;
  OpenHandles++;
  return IoError_OK;
}

IoError MpkArchive::DriverGetSize(uint32_t id, int64_t* outSize) {
  auto it = IdsToFiles.find(id);
  if (it == IdsToFiles.end()) {
    ImpLog(LL_Debug, LC_IO, "MPK GetSize: %d not found in \"%s\"\n", id,
           MountPoint);
    return IoError_NotFound;
  } else {
    MpkMetaEntry* entry = &TOC[it->second];
    ImpLogSlow(LL_Trace, LC_IO, "MPK GetSize: %d found for %d in \"%s\"\n",
               entry->UncompressedSize, id, MountPoint);
    *outSize = entry->UncompressedSize;
    return IoError_OK;
  }
}

IoError MpkArchive::DriverCanStream(uint32_t id, bool* outResult) {
  auto it = IdsToFiles.find(id);
  if (it == IdsToFiles.end()) {
    ImpLog(LL_Debug, LC_IO, "MPK CanStream: %d not found in \"%s\"\n", id,
           MountPoint);
    return IoError_NotFound;
  }
  *outResult = true;
  return IoError_OK;
}

IoError MpkArchive::DriverSlurp(uint32_t id, void* outBuffer) {
  // TODO implement this for compressed content? (zlib finalize or sth)
  ImpLog(LL_Error, LC_IO,
         "MPK DriverSlurp not implemented, should not have been called\n");
  return IoError_Fail;
}

IoError MpkArchive::GetName(uint32_t id, char* outName) {
  auto it = IdsToFiles.find(id);
  if (it == IdsToFiles.end()) {
    ImpLog(LL_Debug, LC_IO, "MPK GetName: %d not found in \"%s\"\n", id,
           MountPoint);
    return IoError_NotFound;
  } else {
    MpkMetaEntry* entry = &TOC[it->second];
    ImpLogSlow(LL_Trace, LC_IO, "MPK GetName: %s found for %d in \"%s\"\n",
               entry->Name, id, MountPoint);
    strncpy(outName, entry->Name, std::min(VfsMaxPath, MpkMaxPath));
    outName[VfsMaxPath - 1] = '\0';
    return IoError_OK;
  }
}

IoError MpkArchive::GetId(const char* path, uint32_t* outId) {
  auto it = NamesToIds.find(path);
  if (it == NamesToIds.end()) {
    ImpLog(LL_Debug, LC_IO, "MPK GetId: %s not found in \"%s\"\n", path,
           MountPoint);
    return IoError_NotFound;
  } else {
    *outId = it->second;
    ImpLogSlow(LL_Trace, LC_IO, "MPK GetId: %d found for %s in \"%s\"\n",
               *outId, path, MountPoint);
    return IoError_OK;
  }
}

IoError MpkArchive::EnumerateNext(uint32_t* inoutIterator,
                                  VfsFileInfo* outFileInfo) {
  if (FileCount <= *inoutIterator || *inoutIterator < 0) {
    ImpLogSlow(LL_Trace, LC_IO,
               "MPK EnumerateNext: %d out of bounds in \"%s\"\n",
               *inoutIterator, MountPoint);
    return IoError_Eof;
  }
  outFileInfo->Id = TOC[*inoutIterator].Id;
  strncpy(outFileInfo->Name, TOC[*inoutIterator].Name,
          std::min(VfsMaxPath, MpkMaxPath));
  outFileInfo->Name[VfsMaxPath - 1] = '\0';
  *inoutIterator = *inoutIterator + 1;
  return IoError_OK;
}

IoError MpkArchive::EnumerateStart(uint32_t* outIterator,
                                   VfsFileInfo* outFileInfo) {
  *outIterator = 0;
  return EnumerateNext(outIterator, outFileInfo);
}

void MpkRegisterDriver() {
  if (!registered) {
    VfsArchive::RegisterDriver(MpkMount);
    registered = true;
    ImpLog(LL_Info, LC_IO, "Registered MPK VFS driver\n");
  } else {
    ImpLog(LL_Warning, LC_IO, "MPK VFS driver registered twice\n");
  }
}

}  // namespace Impacto