#include "lnk4archive.h"

#include "../log.h"
#include "uncompressedstream.h"
#ifndef IMPACTO_DISABLE_MSPACK
#include "lzxstream.h"
#endif
#include "vfs.h"
#include "../util.h"
#include <SDL_endian.h>

namespace Impacto {
namespace Io {

struct Lnk4MetaEntry : FileMeta {
  int64_t Offset;
};

Lnk4Archive::~Lnk4Archive() {
  if (TOC) delete[] TOC;
}

IoError Lnk4Archive::Open(FileMeta* file, Stream** outStream) {
  Lnk4MetaEntry* entry = (Lnk4MetaEntry*)file;
#ifndef IMPACTO_DISABLE_MSPACK
  IoError err =
      LzxStream::Create(BaseStream, entry->Offset, entry->Size, outStream);
  if (err != IoError_OK) {
    err = UncompressedStream::Create(BaseStream, entry->Offset, entry->Size,
                                     outStream);
  }
#else
  IoError err = UncompressedStream::Create(BaseStream, entry->Offset,
                                           entry->Size, outStream);
#endif
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "LNK4 file open failed for file \"{:s}\" in archive \"{:s}\"\n",
           entry->FileName, BaseStream->Meta.FileName);
  }
  return err;
}

IoError Lnk4Archive::Create(Stream* stream, VfsArchive** outArchive) {
  ImpLog(LogLevel::Trace, LogChannel::IO, "Trying to mount \"{:s}\" as LNK4\n",
         stream->Meta.FileName);

  Lnk4Archive* result = 0;
  uint32_t* rawToc = 0;

  uint32_t offsetBlockSize = 2048;
  uint32_t blockSize = 1024;

  uint32_t const magic = 0x4C4E4B34;

  uint32_t const tocOffset = 8;

  uint32_t maxFileCount;
  uint32_t fileCount;
  uint32_t dataOffset;
  char fileName[6];

  if (ReadBE<uint32_t>(stream) != magic) {
    ImpLog(LogLevel::Trace, LogChannel::IO, "Not LNK4\n");
    goto fail;
  }

  dataOffset = ReadLE<uint32_t>(stream);
  if (dataOffset < tocOffset) {
    ImpLog(LogLevel::Error, LogChannel::IO, "LNK4 header too short\n");
    goto fail;
  }

  maxFileCount = (dataOffset - tocOffset) / 8;

  rawToc = (uint32_t*)ImpStackAlloc(dataOffset - tocOffset);
  if (stream->Read(rawToc, dataOffset - tocOffset) != dataOffset - tocOffset)
    goto fail;

  fileCount = 0;
  for (uint32_t* it = rawToc; it < rawToc + (maxFileCount * 2); it += 2) {
    // first file starts at 0
    if (SDL_SwapLE32(*it) == 0 && it != rawToc) break;
    fileCount++;
  }

  result = new Lnk4Archive;
  result->BaseStream = stream;
  result->NamesToIds.reserve(fileCount);
  result->IdsToFiles.reserve(fileCount);
  result->TOC = new Lnk4MetaEntry[fileCount];

  for (uint32_t i = 0; i < fileCount; i++) {
    snprintf(fileName, 6, "%05i", i);
    result->TOC[i].FileName = fileName;
    result->TOC[i].Id = i;
    result->TOC[i].Offset =
        SDL_SwapLE32(rawToc[i * 2]) * offsetBlockSize + dataOffset;
    result->TOC[i].Size = SDL_SwapLE32(rawToc[i * 2 + 1]) * blockSize;
    result->IdsToFiles[i] = &result->TOC[i];
    result->NamesToIds[result->TOC[i].FileName] = i;
  }

  ImpStackFree(rawToc);

  result->IsInit = true;
  *outArchive = result;
  return IoError_OK;

fail:
  stream->Seek(0, RW_SEEK_SET);
  if (result) delete result;
  if (rawToc) ImpStackFree(rawToc);
  return IoError_Fail;
}

}  // namespace Io
}  // namespace Impacto