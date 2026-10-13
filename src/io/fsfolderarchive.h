#pragma once

#include "vfsarchive.h"
#include <string>
#include <vector>

namespace Impacto {
namespace Io {

struct TextMetaEntry;

struct FolderArchiveParameters {
  std::optional<std::string> OrderFilePath;
  std::optional<std::string> WhitelistPattern;
};

class FSFolderArchive : public VfsArchive {
 public:
  IoError Open(FileMeta* file, Stream** outStream) override;
  IoError GetCurrentSize(FileMeta* file, int64_t& outSize) override;

  static IoError Create(Stream* stream, VfsArchive** outArchive,
                        std::optional<FolderArchiveParameters> params);

 private:
  static IoError SortTOCLexicographically(
      FSFolderArchive* result, std::filesystem::path const& rootPath);
  static IoError SortTOCByOrderFile(FSFolderArchive* result,
                                    VfsArchive** orderLines);
  std::vector<TextMetaEntry> TOC;
};

}  // namespace Io
}  // namespace Impacto