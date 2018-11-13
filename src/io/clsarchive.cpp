#include "vfsarchive.h"

#include "../log.h"
#include "physicalfilestream.h"
#include "vfs.h"
#include "../util.h"
#include <sstream>
#include <SDL_rwops.h>

namespace Impacto {
namespace Io {

class ClsArchive : public VfsArchive {
 public:
  ~ClsArchive();
  IoError Open(FileMeta* file, InputStream** outStream) override;
  IoError GetCurrentSize(FileMeta* file, int64_t* outSize) override;

 private:
  FileMeta* TOC = 0;
  std::string BasePath;

  static bool _registered;
  static IoError Create(InputStream* stream, VfsArchive** outArchive);
};

ClsArchive::~ClsArchive() {
  if (TOC) delete[] TOC;
}

IoError ClsArchive::Open(FileMeta* file, InputStream** outStream) {
  IoError err =
      PhysicalFileStream::Create(BasePath + file->FileName, outStream);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO,
           "CLS file open failed for file \"%s\" in archive \"%s\"\n",
           file->FileName.c_str(), BaseStream->Meta.FileName.c_str());
  }
  return err;
}

IoError ClsArchive::GetCurrentSize(FileMeta* file, int64_t* outSize) {
  std::string path = BasePath + file->FileName;
  SDL_RWops* rw = SDL_RWFromFile(path.c_str(), "rb");
  if (!rw) {
    ImpLog(LL_Error, LC_IO,
           "CLS file open (to get size) failed for file \"%s\" in archive "
           "\"%s\"\n",
           file->FileName.c_str(), BaseStream->Meta.FileName.c_str());
    return IoError_Fail;
  }
  *outSize = SDL_RWsize(rw);
  SDL_RWclose(rw);
  if (*outSize < 0) {
    ImpLog(LL_Error, LC_IO,
           "CLS getting size failed for file \"%s\" in archive \"%s\"\n",
           file->FileName.c_str(), BaseStream->Meta.FileName.c_str());
    return IoError_Fail;
  }
  return IoError_OK;
}

IoError ClsArchive::Create(InputStream* stream, VfsArchive** outArchive) {
  ImpLog(LL_Trace, LC_IO, "Trying to mount \"%s\" as CLS\n",
         stream->Meta.FileName.c_str());

  if (!StringEndsWithCi(stream->Meta.FileName, ".cls")) {
    ImpLog(LL_Trace, LC_IO, "Not CLS\n");
    return IoError_Fail;
  }

  std::string content(stream->Meta.Size, '\0');
  int64_t size = stream->Read(&content[0], stream->Meta.Size);
  content.resize(size);
  int64_t maxFileCount = std::count(content.begin(), content.end(), '\n');

  ClsArchive* result = 0;

  result = new ClsArchive;
  result->BaseStream = stream;
  result->NamesToIds.reserve(maxFileCount);
  result->IdsToFiles.reserve(maxFileCount);
  result->TOC = new FileMeta[maxFileCount];

  result->BasePath =
      VfsArchiveBasePath + "/" +
      stream->Meta.FileName.substr(
          0, stream->Meta.FileName.length() - std::string(".cls").length()) +
      "/";

  uint32_t nextId = 0;

  std::istringstream ss(content, std::ios::in);
  for (std::string line; std::getline(ss, line);) {
    if (!line.empty()) {
      if (line[line.size() - 1] == '\r') line.pop_back();
      result->TOC[nextId].FileName = line;
      result->TOC[nextId].Id = nextId;
      result->TOC[nextId].Size = -1;
      result->NamesToIds[line] = nextId;
      result->IdsToFiles[nextId] = &result->TOC[nextId];
    }
    nextId++;
  }

  result->IsInit = true;
  *outArchive = result;
  return IoError_OK;

fail:
  stream->Seek(0, RW_SEEK_SET);
  if (result) delete result;
  return IoError_Fail;
}

bool ClsArchive::_registered = VfsRegisterArchiver(ClsArchive::Create);

}  // namespace Io
}  // namespace Impacto