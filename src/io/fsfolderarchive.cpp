#include "fsfolderarchive.h"

#include "../log.h"
#include "io.h"
#include "physicalfilestream.h"
#include "vfs.h"
#include "../util.h"
#include <sstream>
#include <filesystem>

namespace Impacto {
namespace Io {

IoError FSFolderArchive::Open(FileMeta* file, Stream** outStream) {
  TextMetaEntry* entry = (TextMetaEntry*)file;
  IoError err = PhysicalFileStream::Create(entry->FullPath, outStream);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "FSFolderArchive file open failed for file \"{:s}\" in archive "
           "\"{:s}\"\n",
           entry->FullPath, BaseStream->Meta.FileName);
  }
  return err;
}

IoError FSFolderArchive::GetCurrentSize(FileMeta* file, int64_t& outSize) {
  TextMetaEntry* entry = (TextMetaEntry*)file;
  std::error_code ec;
  outSize = std::filesystem::file_size(entry->FullPath, ec);
  if (ec) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "FSFolderArchive getting size failed for file \"{:s}\" in archive "
           "\"{:s}\"\nerror: {:s}\n",
           entry->FullPath, BaseStream->Meta.FileName, ec.message());
    return IoError_Fail;
  }
  return IoError_OK;
}

IoError FSFolderArchive::Create(Stream* stream, VfsArchive** outArchive) {
  namespace fs = std::filesystem;

  ImpLog(LogLevel::Trace, LogChannel::IO,
         "Trying to mount \"{:s}\" as filesystem folder archive\n",
         stream->Meta.FileName);

  FSFolderArchive* result;

  result = new FSFolderArchive;
  result->BaseStream = stream;

  const auto pathLessComparator = [](TextMetaEntry const& a,
                                     TextMetaEntry const& b) {
    const auto toLowerPath = [](std::string p) {
      std::transform(p.begin(), p.end(), p.begin(), [](unsigned char c) {
        if (c >= 'A' && c <= 'Z') return static_cast<char>(std::tolower(c));
        return static_cast<char>(c);
      });
      return fs::path(p);
    };
    return toLowerPath(a.FullPath) < toLowerPath(b.FullPath);
  };
  auto iterateDirectory =
      [result](auto&& self, std::filesystem::path folderPath) -> IoError {
    std::error_code ec;
    for (const auto& dirEntry : fs::directory_iterator(folderPath, ec)) {
      if (dirEntry.is_directory()) {
        if (auto code = self(self, dirEntry.path()); code != IoError_OK) {
          return code;
        }
        continue;
      }

      const auto path = dirEntry.path();
      ec = std::error_code{};
      TextMetaEntry entry;
      entry.FullPath = path.string();
      entry.FileName = path.filename().string();
      entry.Size = dirEntry.file_size(ec);
      if (ec) entry.Size = -1;
      result->TOC.push_back(entry);
    }
    if (ec) {
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to iterate over directory {} with error {}\n", folderPath,
             ec.message());
      if (result) delete result;
      return IoError_Fail;
    }
    return IoError_OK;
  };
  if (auto code = iterateDirectory(iterateDirectory, stream->Meta.FileName);
      code != IoError_OK) {
    return code;
  }
  std::sort(result->TOC.begin(), result->TOC.end(), pathLessComparator);

  uint32_t id = 0;
  for (auto&& entry : result->TOC) {
    entry.Id = id;
    result->IdsToFiles[id] = &entry;
    result->NamesToIds[entry.FileName] = id;
    id++;
  }

  result->IsInit = true;
  *outArchive = result;
  return IoError_OK;
}

}  // namespace Io
}  // namespace Impacto