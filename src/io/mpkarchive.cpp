#include "mpkarchive.h"

#include "../log.h"
#include <flat_hash_map.hpp>
#include "zlibstream.h"
#include "uncompressedstream.h"
#include "vfs.h"

namespace Impacto {
namespace Io {

int const MpkMaxPath = 224;

struct MpkMetaEntry : FileMeta {
  int64_t Offset;
  int64_t CompressedSize;
  bool Compressed;
};

MpkArchive::~MpkArchive() {
  if (TOC) delete[] TOC;
}

IoError MpkArchive::Open(FileMeta* file, Stream** outStream) {
  MpkMetaEntry* entry = (MpkMetaEntry*)file;
  IoError err;
  if (entry->Compressed) {
    err = ZlibStream::Create(BaseStream, entry->Offset, entry->CompressedSize,
                             entry->Size, outStream);

  } else {
    err = UncompressedStream::Create(BaseStream, entry->Offset, entry->Size,
                                     outStream);
  }
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO,
           "MPK file open failed for file \"%s\" in archive \"%s\" "
           "(compression %d)\n",
           entry->FileName.c_str(), BaseStream->Meta.FileName.c_str(),
           entry->Compressed);
  }
  return err;
}

IoError MpkArchive::Create(Stream* stream, VfsArchive** outArchive) {
  ImpLog(LL_Trace, LC_IO, "Trying to mount \"%s\" as MPK\n",
         stream->Meta.FileName.c_str());

  MpkArchive* result = 0;

  uint32_t const magic = 0x4D504B00;

  uint32_t FileCount;
  uint16_t MinorVersion;
  uint16_t MajorVersion;
  char name[MpkMaxPath];

  if (ReadBE<uint32_t>(stream) != magic) {
    ImpLog(LL_Trace, LC_IO, "Not an MPK\n");
    goto fail;
  }

  MinorVersion = ReadLE<uint16_t>(stream);
  MajorVersion = ReadLE<uint16_t>(stream);
  // TODO support v1
  if (MinorVersion != 0 || MajorVersion != 2) {
    ImpLog(LL_Trace, LC_IO, "Unsupported MPK version %d.%d\n", MajorVersion,
           MinorVersion);
    goto fail;
  }

  result = new MpkArchive;
  result->BaseStream = stream;
  FileCount = ReadLE<uint32_t>(stream);
  result->NamesToIds.reserve(FileCount);
  result->IdsToFiles.reserve(FileCount);
  result->TOC = new MpkMetaEntry[FileCount];

  stream->Seek(0x40, RW_SEEK_SET);
  for (uint32_t i = 0; i < FileCount; i++) {
    uint32_t Compression = ReadLE<uint32_t>(stream);
    uint32_t Id = ReadLE<uint32_t>(stream);

    if (Compression != 1 && Compression != 0) {
      ImpLog(LL_Error, LC_IO, "Unknown MPK compression type %d on file %d\n",
             Id, Compression);
      stream->Seek(0x100 - 8, RW_SEEK_SET);
      continue;
    }

    MpkMetaEntry* entry = &result->TOC[i];
    entry->Compressed = Compression;
    entry->Id = Id;
    entry->Offset = ReadLE<uint64_t>(stream);
    entry->CompressedSize = ReadLE<uint64_t>(stream);
    entry->Size = ReadLE<uint64_t>(stream);
    stream->Read(name, MpkMaxPath);
    name[MpkMaxPath - 1] = '\0';
    entry->FileName = std::string(name);

    if (result->IdsToFiles.find(Id) != result->IdsToFiles.end()) {
      ImpLog(LL_Error, LC_IO, "Duplicate MPK file ID %d\n", Id);
      continue;
    }
    if (!entry->Offset) {
      ImpLog(LL_Error, LC_IO, "Reached end of ToC\n");
      break;
    }

    result->IdsToFiles[Id] = entry;
    result->NamesToIds[entry->FileName] = Id;
  }

  result->IsInit = true;
  *outArchive = result;
  return IoError_OK;

fail:
  stream->Seek(0, RW_SEEK_SET);
  if (result) delete result;
  return IoError_Fail;
}

}  // namespace Io
}  // namespace Impacto