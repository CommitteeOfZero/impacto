#pragma once

#include "vfsarchive.h"

namespace Impacto {
namespace Io {

struct MpkMetaEntry;

class MpkArchive : public VfsArchive {
 public:
  ~MpkArchive();
  IoError Open(FileMeta *file, Stream **outStream) override;

  static IoError Create(Stream *stream, VfsArchive **outArchive);

 private:
  MpkMetaEntry *TOC = 0;
};

}  // namespace Io
}  // namespace Impacto