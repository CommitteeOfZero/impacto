#include "textarchive.h"

#include "../log.h"
#include "io.h"
#include "physicalfilestream.h"
#include "vfs.h"
#include "../util.h"
#include <sstream>
#include <filesystem>

namespace Impacto {
namespace Io {

enum TextArchiveType { CLS, MLP, TextMPK };
auto format_as(TextArchiveType type) -> std::string {
  switch (type) {
    case CLS:
      return "CLS";
    case MLP:
      return "MLP";
    case TextMPK:
      return "TextMPK";
  }
  return "Unknown";
}

struct TextMetaEntry : FileMeta {
  std::string FullPath;
};

TextArchive::~TextArchive() {
  if (TOC) delete[] TOC;
}

IoError TextArchive::Open(FileMeta* file, Stream** outStream) {
  TextMetaEntry* entry = (TextMetaEntry*)file;
  IoError err = PhysicalFileStream::Create(entry->FullPath, outStream);
  if (err != IoError_OK) {
    ImpLog(
        LogLevel::Error, LogChannel::IO,
        "TextArchive file open failed for file \"{:s}\" in archive \"{:s}\"\n",
        entry->FullPath, BaseStream->Meta.FileName);
  }
  return err;
}

IoError TextArchive::GetCurrentSize(FileMeta* file, int64_t& outSize) {
  TextMetaEntry* entry = (TextMetaEntry*)file;
  std::error_code ec;
  outSize = std::filesystem::file_size(entry->FullPath, ec);
  if (ec) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "TextArchive getting size failed for file \"{:s}\" in archive "
           "\"{:s}\"\nerror: {:s}\n",
           entry->FullPath, BaseStream->Meta.FileName, ec.message());
    return IoError_Fail;
  }
  return IoError_OK;
}

IoError TextArchive::Create(Stream* stream, VfsArchive** outArchive) {
  ImpLog(LogLevel::Trace, LogChannel::IO,
         "Trying to mount \"{:s}\" as text archive\n", stream->Meta.FileName);

  std::istringstream ss;
  std::string content;
  std::string basePath;

  TextArchiveType type;

  int64_t size;
  int64_t maxFileCount;
  TextArchive* result;
  uint32_t lineId;

  result = 0;

  if (StringEndsWithCi(stream->Meta.FileName, ".cls")) {
    type = CLS;
  } else if (StringEndsWithCi(stream->Meta.FileName, ".mlp")) {
    type = MLP;
  } else if (StringEndsWithCi(stream->Meta.FileName, ".mpk")) {
    type = TextMPK;
    if (ReadBE<uint32_t>(stream) == 0x4D504B00u) {
      ImpLog(LogLevel::Trace, LogChannel::IO, "Actually a binary MPK\n");
      goto fail;
    }
    stream->Seek(0, RW_SEEK_SET);
  } else {
    ImpLog(LogLevel::Trace, LogChannel::IO, "Not a text archive\n");
    goto fail;
  }

  content.resize(stream->Meta.Size);
  size = stream->Read(&content[0], stream->Meta.Size);
  content.resize(size);
  maxFileCount = std::count(content.begin(), content.end(), '\n') + 1;

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

  lineId = 0;

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
      size_t secondColLength = line.find(',', firstColLength + 1);
      if (secondColLength == std::string::npos ||
          secondColLength == line.length() - 1)
        continue;

      std::string fullPath = line.substr(0, firstColLength);
      id = std::atoi(&line[secondColLength + 1]);
      result->TOC[lineId].FullPath = basePath + fullPath;
      result->TOC[lineId].FileName =
          line.substr(firstColLength + 1, secondColLength);
    } else {
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Archive {:s} could not be mounted as type {:d} is unknown\n",
             stream->Meta.FileName, to_underlying(type));
      return IoError_Fail;
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

}  // namespace Io
}  // namespace Impacto