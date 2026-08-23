#pragma once

#include "vfsarchive.h"
#include <string>
#include <vector>

namespace Impacto {
namespace Io {

struct TextMetaEntry : FileMeta {
  std::string FullPath;
};

class FSFolderArchive : public VfsArchive {
 public:
  IoError Open(FileMeta* file, Stream** outStream) override;
  IoError GetCurrentSize(FileMeta* file, int64_t& outSize) override;

  static IoError Create(Stream* stream, VfsArchive** outArchive);

 private:
  std::vector<TextMetaEntry> TOC;
};

}  // namespace Io
}  // namespace Impacto