#include "afsarchive.h"

#include "../log.h"
#include "uncompressedstream.h"
#include "vfs.h"
#include "../util.h"
#include <SDL_endian.h>

namespace Impacto {
namespace Io {

struct AfsMetaEntry : FileMeta {
  int64_t Offset;
};

AfsArchive::~AfsArchive() {
  if (TOC) delete[] TOC;
}

IoError AfsArchive::Open(FileMeta* file, InputStream** outStream) {
  AfsMetaEntry* entry = (AfsMetaEntry*)file;
  IoError err = UncompressedStream::Create(BaseStream, entry->Offset,
                                           entry->Size, outStream);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO,
           "AFS file open failed for file \"%s\" in archive \"%s\"\n",
           entry->FileName.c_str(), BaseStream->Meta.FileName.c_str());
  }
  return err;
}

IoError AfsArchive::Create(InputStream* stream, VfsArchive** outArchive) {
  ImpLog(LL_Trace, LC_IO, "Trying to mount \"%s\" as AFS\n",
         stream->Meta.FileName.c_str());

  AfsArchive* result = 0;
  uint32_t* rawToc = 0;

  uint32_t const magic = 0x41465300;

  uint32_t const tocEntrySize = 8;

  uint32_t fileCount;
  char fileName[6];

  if (ReadBE<uint32_t>(stream) != magic) {
    ImpLog(LL_Trace, LC_IO, "Not AFS\n");
    goto fail;
  }

  fileCount = ReadLE<uint32_t>(stream);

  rawToc = (uint32_t*)ImpStackAlloc(fileCount * tocEntrySize);
  if (stream->Read(rawToc, fileCount * tocEntrySize) !=
      fileCount * tocEntrySize)
    goto fail;

  result = new AfsArchive;
  result->BaseStream = stream;
  result->NamesToIds.reserve(fileCount);
  result->IdsToFiles.reserve(fileCount);
  result->TOC = new AfsMetaEntry[fileCount];

  for (uint32_t i = 0; i < fileCount; i++) {
    snprintf(fileName, 6, "%05i", i);
    result->TOC[i].FileName = fileName;
    result->TOC[i].Id = i;
    result->TOC[i].Offset = SDL_SwapLE32(rawToc[i * 2]);
    result->TOC[i].Size = SDL_SwapLE32(rawToc[i * 2 + 1]);
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