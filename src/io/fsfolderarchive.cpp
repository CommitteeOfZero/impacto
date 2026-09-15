#include "fsfolderarchive.h"

#include "../log.h"
#include "io.h"
#include "physicalfilestream.h"
#include "vfs.h"
#include "../util.h"
#include <sstream>
#include <filesystem>
#include <regex>
#include "memorymappedfilestream.h"
#include "textarchive.h"

namespace Impacto {
namespace Io {

struct TextMetaEntry : FileMeta {
  std::string FullPath;
};

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

IoError FSFolderArchive::SortTOCLexicographically(
    FSFolderArchive* result, std::filesystem::path const& rootPath) {
  const auto pathLessComparator = [](TextMetaEntry const& a,
                                     TextMetaEntry const& b) {
    const auto toLowerPath = [](std::string p) {
      std::transform(p.begin(), p.end(), p.begin(), [](unsigned char c) {
        if (c >= 'A' && c <= 'Z') return static_cast<char>(std::tolower(c));
        return static_cast<char>(c);
      });
      return std::filesystem::path(p);
    };
    return toLowerPath(a.FullPath) < toLowerPath(b.FullPath);
  };

  std::sort(result->TOC.begin(), result->TOC.end(), pathLessComparator);
  return IoError_OK;
}

IoError FSFolderArchive::SortTOCByOrderFile(FSFolderArchive* result,
                                            VfsArchive** archive) {
  TextArchive* txtArch = dynamic_cast<TextArchive*>(*archive);
  if (!txtArch) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "SortTOCByOrderFile: order file is not a valid TextArchive\n");
    return IoError_Fail;
  }

  std::unordered_map<std::string, TextMetaEntry*> nameToEntry;
  nameToEntry.reserve(result->TOC.size());
  for (auto& entry : result->TOC) {
    std::string stem = std::filesystem::path(entry.FileName).stem().string();
    nameToEntry[stem] = &entry;
  }

  std::vector<TextMetaEntry> orderedToc;
  orderedToc.reserve(txtArch->IdsToFiles.size());

  for (auto const& [id, fileMeta] : txtArch->IdsToFiles) {
    if (!fileMeta) continue;

    auto foundIt = nameToEntry.find(fileMeta->FileName);
    if (foundIt == nameToEntry.end()) continue;

    orderedToc.push_back(*foundIt->second);
  }

  result->TOC = std::move(orderedToc);
  return IoError_OK;
}

IoError FSFolderArchive::Create(Stream* stream, VfsArchive** outArchive,
                                std::optional<FolderArchiveParameters> params) {
  namespace fs = std::filesystem;

  ImpLog(LogLevel::Trace, LogChannel::IO,
         "Trying to mount \"{:s}\" as filesystem folder archive\n",
         stream->Meta.FileName);

  FSFolderArchive* result = new FSFolderArchive;
  result->BaseStream = stream;
  std::optional<std::regex> whitelistRegex;

  if (params && params->whitelistPattern) {
    try {
      whitelistRegex.emplace(*params->whitelistPattern,
                             std::regex::ECMAScript | std::regex::icase);
    } catch (std::regex_error const& e) {
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Invalid whitelist pattern \"{:s}\": {:s}\n",
             *params->whitelistPattern, e.what());
      return IoError_Fail;
    }
  }

  auto iterateDirectory = [result, &whitelistRegex](
                              auto&& self, fs::path folderPath) -> IoError {
    std::error_code ec;
    for (const auto& dirEntry : fs::directory_iterator(folderPath, ec)) {
      if (dirEntry.is_directory()) {
        if (auto code = self(self, dirEntry.path()); code != IoError_OK) {
          return code;
        }
        continue;
      }

      const auto path = dirEntry.path();
      std::string fullPath = path.string();

      if (whitelistRegex && !std::regex_match(fullPath, *whitelistRegex)) {
        continue;
      }

      ec = std::error_code{};
      TextMetaEntry entry;
      entry.FullPath = fullPath;
      entry.FileName = path.filename().string();
      entry.Size = dirEntry.file_size(ec);
      if (ec) entry.Size = -1;
      result->TOC.push_back(entry);
    }
    if (ec) {
      ImpLog(LogLevel::Error, LogChannel::IO,
             "Failed to iterate over directory {} with error {}\n", folderPath,
             ec.message());
      return IoError_Fail;
    }
    return IoError_OK;
  };
  if (auto code = iterateDirectory(iterateDirectory, stream->Meta.FileName);
      code != IoError_OK) {
    return code;
  }

  if (params && params->orderFilePath) {
    Stream* orderFile;
    IoError err;
    std::string const& orderFileName = *(params->orderFilePath);
#ifndef IMPACTO_DISABLE_MMAP
    err = MemoryMappedFileStream<AccessMode::read>::Create(orderFileName,
                                                           &orderFile);
#else
    err = PhysicalFileStream::Create(orderFileName, &orderFile);
#endif
    VfsArchive* orderFileArchive = nullptr;
    TextArchive::Create(orderFile, &orderFileArchive);
    if (auto code = SortTOCByOrderFile(result, &orderFileArchive);
        code != IoError_OK) {
      delete result;
      return code;
    }
  } else if (auto code =
                 SortTOCLexicographically(result, stream->Meta.FileName);
             code != IoError_OK) {
    delete result;
    return code;
  }

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