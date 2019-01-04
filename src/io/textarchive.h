#pragma once

#include "vfsarchive.h"
#include <string>

namespace Impacto {
namespace Io {

struct TextMetaEntry;

class TextArchive : public VfsArchive {
 public:
  ~TextArchive();
  IoError Open(FileMeta* file, InputStream** outStream) override;
  IoError GetCurrentSize(FileMeta* file, int64_t* outSize) override;

  static IoError Create(InputStream* stream, VfsArchive** outArchive);

 private:
  TextMetaEntry* TOC = 0;
  std::string BasePath;
};

}  // namespace Io
}  // namespace Impacto