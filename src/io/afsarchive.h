#pragma once

#include "vfsarchive.h"

namespace Impacto {
namespace Io {

struct AfsMetaEntry;

class AfsArchive : public VfsArchive {
 public:
  ~AfsArchive();
  IoError Open(FileMeta* file, Stream** outStream) override;

  static IoError Create(Stream* stream, VfsArchive** outArchive);

 private:
  AfsMetaEntry* TOC = 0;
};

}  // namespace Io
}  // namespace Impacto