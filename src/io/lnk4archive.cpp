#include "vfsarchive.h"

#include "../log.h"
#include "uncompressedstream.h"
#include "vfs.h"
#include "../util.h"
#include <SDL_endian.h>

namespace Impacto {
namespace Io {

struct Lnk4MetaEntry : FileMeta {
  int64_t Offset;
};

class Lnk4Archive : public VfsArchive {
 public:
  ~Lnk4Archive();
  IoError Open(FileMeta* file, InputStream** outStream) override;

 private:
  Lnk4MetaEntry* TOC = 0;
  static bool _registered;
  static IoError Create(InputStream* stream, VfsArchive** outArchive);
};

Lnk4Archive::~Lnk4Archive() {
  if (TOC) delete[] TOC;
}

IoError Lnk4Archive::Open(FileMeta* file, InputStream** outStream) {
  Lnk4MetaEntry* entry = (Lnk4MetaEntry*)file;
  IoError err = UncompressedStream::Create(BaseStream, entry->Offset,
                                           entry->Size, outStream);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO,
           "LNK4 file open failed for file \"%s\" in archive \"%s\"\n",
           entry->FileName.c_str(), BaseStream->Meta.FileName.c_str());
  }
  return err;
}

IoError Lnk4Archive::Create(InputStream* stream, VfsArchive** outArchive) {
  ImpLog(LL_Trace, LC_IO, "Trying to mount \"%s\" as LNK4\n",
         stream->Meta.FileName.c_str());

  Lnk4Archive* result = 0;
  uint32_t* rawToc = 0;

  uint32_t offsetBlockSize = 2048;
  uint32_t blockSize = 1024;

  uint32_t const magic = 0x4C4E4B34;
  if (ReadBE<uint32_t>(stream) != magic) {
    ImpLog(LL_Trace, LC_IO, "Not LNK4\n");
    goto fail;
  }
  uint32_t const tocOffset = 8;
  uint32_t dataOffset = ReadLE<uint32_t>(stream);
  if (dataOffset < tocOffset) {
    ImpLog(LL_Error, LC_IO, "LNK4 header too short\n");
    goto fail;
  }

  uint32_t maxFileCount = (dataOffset - tocOffset) / 8;

  rawToc = (uint32_t*)ImpStackAlloc(dataOffset - tocOffset);
  if (stream->Read(rawToc, dataOffset - tocOffset) != dataOffset - tocOffset)
    goto fail;

  uint32_t fileCount = 0;
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

  char fileName[6];
  for (uint32_t i = 0; i < fileCount; i++) {
    snprintf(fileName, 6, "%5i", i);
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

bool Lnk4Archive::_registered = VfsRegisterArchiver(Lnk4Archive::Create);

}  // namespace Io
}  // namespace Impacto