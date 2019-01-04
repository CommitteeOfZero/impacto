#pragma once

#include "vfsarchive.h"

namespace Impacto {
namespace Io {

struct MpkMetaEntry;

class MpkArchive : public VfsArchive {
 public:
  ~MpkArchive();
  IoError Open(FileMeta *file, InputStream **outStream) override;

  static IoError Create(InputStream *stream, VfsArchive **outArchive);

 private:
  MpkMetaEntry *TOC = 0;
};

}  // namespace Io
}  // namespace Impacto