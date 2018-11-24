#include "vfsarchive.h"

#include "../log.h"
#include "physicalfilestream.h"
#include "vfs.h"
#include "../util.h"
#include <sstream>
#include <SDL_rwops.h>

namespace Impacto {
namespace Io {

enum TextArchiveType { CLS, MLP, TextMPK };

struct TextMetaEntry : FileMeta {
  std::string FullPath;
};

class TextArchive : public VfsArchive {
 public:
  ~TextArchive();
  IoError Open(FileMeta* file, InputStream** outStream) override;
  IoError GetCurrentSize(FileMeta* file, int64_t* outSize) override;

 private:
  TextMetaEntry* TOC = 0;
  std::string BasePath;

  static bool _registered;
  static IoError Create(InputStream* stream, VfsArchive** outArchive);
};

TextArchive::~TextArchive() {
  if (TOC) delete[] TOC;
}

IoError TextArchive::Open(FileMeta* file, InputStream** outStream) {
  TextMetaEntry* entry = (TextMetaEntry*)file;
  IoError err = PhysicalFileStream::Create(entry->FullPath, outStream);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO,
           "TextArchive file open failed for file \"%s\" in archive \"%s\"\n",
           entry->FullPath.c_str(), BaseStream->Meta.FileName.c_str());
  }
  return err;
}

IoError TextArchive::GetCurrentSize(FileMeta* file, int64_t* outSize) {
  TextMetaEntry* entry = (TextMetaEntry*)file;
  SDL_RWops* rw = SDL_RWFromFile(entry->FullPath.c_str(), "rb");
  if (!rw) {
    ImpLog(
        LL_Error, LC_IO,
        "TextArchive file open (to get size) failed for file \"%s\" in archive "
        "\"%s\"\n",
        entry->FullPath.c_str(), BaseStream->Meta.FileName.c_str());
    return IoError_Fail;
  }
  *outSize = SDL_RWsize(rw);
  SDL_RWclose(rw);
  if (*outSize < 0) {
    ImpLog(
        LL_Error, LC_IO,
        "TextArchive getting size failed for file \"%s\" in archive \"%s\"\n",
        entry->FullPath.c_str(), BaseStream->Meta.FileName.c_str());
    return IoError_Fail;
  }
  return IoError_OK;
}

IoError TextArchive::Create(InputStream* stream, VfsArchive** outArchive) {
  ImpLog(LL_Trace, LC_IO, "Trying to mount \"%s\" as text archive\n",
         stream->Meta.FileName.c_str());

  std::istringstream ss;
  std::string content;
  std::string basePath;

  TextArchiveType type;

  if (StringEndsWithCi(stream->Meta.FileName, ".cls")) {
    type = CLS;
  } else if (StringEndsWithCi(stream->Meta.FileName, ".mlp")) {
    type = MLP;
  } else if (StringEndsWithCi(stream->Meta.FileName, ".mpk")) {
    type = TextMPK;
    if (ReadBE<uint32_t>(stream) == 0x4D504B00u) {
      ImpLog(LL_Trace, LC_IO, "Actually a binary MPK\n");
      goto fail;
    }
    stream->Seek(0, RW_SEEK_SET);
  } else {
    ImpLog(LL_Trace, LC_IO, "Not a text archive\n");
    goto fail;
  }

  content.resize(stream->Meta.Size);
  int64_t size = stream->Read(&content[0], stream->Meta.Size);
  content.resize(size);
  int64_t maxFileCount = std::count(content.begin(), content.end(), '\n') + 1;

  TextArchive* result = 0;

  result = new TextArchive;
  result->BaseStream = stream;
  result->NamesToIds.reserve(maxFileCount);
  result->IdsToFiles.reserve(maxFileCount);
  result->TOC = new TextMetaEntry[maxFileCount];

  switch (type) {
    case CLS: {
      basePath =
          stream->Meta.FileName.substr(0, stream->Meta.FileName.length() -
                                              std::string(".cls").length()) +
          "/";
      break;
    }
    case MLP:
    case TextMPK: {
      basePath = "";
      break;
    }
  }

  uint32_t lineId = 0;

  ss = std::istringstream(content, std::ios::in);
  for (std::string line; std::getline(ss, line); lineId++) {
    if (line.empty()) continue;

    if (line[line.size() - 1] == '\r') line.pop_back();
    uint32_t id;

    if (type == CLS) {
      // fullPath
      id = lineId;
      result->TOC[lineId].FileName = line;
      result->TOC[lineId].FullPath = basePath + line;

    } else if (type == MLP) {
      // fullPath,id
      size_t firstColLength = line.find(',');
      if (firstColLength == std::string::npos ||
          firstColLength == line.length() - 1)
        continue;
      std::string fullPath = line.substr(0, firstColLength);
      id = std::atoi(&line[firstColLength + 1]);
      size_t fileNameStart = fullPath.rfind('/') + 1;
      size_t fileNameStart2 = fullPath.rfind('\\') + 1;
      if (fileNameStart2 > fileNameStart) fileNameStart = fileNameStart2;
      result->TOC[lineId].FullPath = basePath + fullPath;
      result->TOC[lineId].FileName = fullPath.substr(fileNameStart);

    } else if (type == TextMPK) {
      // fullPath,name,id
      size_t firstColLength = line.find(',');
      if (firstColLength == std::string::npos ||
          firstColLength == line.length() - 1)
        continue;
      size_t secondColLength = line.find(firstColLength + 1, ',');
      if (secondColLength == std::string::npos ||
          secondColLength == line.length() - 1)
        continue;

      std::string fullPath = line.substr(0, firstColLength);
      id = std::atoi(&line[secondColLength + 1]);
      result->TOC[lineId].FullPath = basePath + fullPath;
      result->TOC[lineId].FileName =
          line.substr(firstColLength + 1, secondColLength);
    }

    result->TOC[lineId].Size = -1;
    result->IdsToFiles[id] = &result->TOC[lineId];
    result->TOC[lineId].Id = id;
    result->NamesToIds[result->TOC[lineId].FileName] = id;
  }

  result->IsInit = true;
  *outArchive = result;
  return IoError_OK;

fail:
  stream->Seek(0, RW_SEEK_SET);
  if (result) delete result;
  return IoError_Fail;
}

bool TextArchive::_registered = VfsRegisterArchiver(TextArchive::Create);

}  // namespace Io
}  // namespace Impacto