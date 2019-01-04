#pragma once

#include "vfsarchive.h"

namespace Impacto {
namespace Io {

struct Lnk4MetaEntry;

class Lnk4Archive : public VfsArchive {
 public:
  ~Lnk4Archive();
  IoError Open(FileMeta* file, InputStream** outStream) override;

  static IoError Create(InputStream* stream, VfsArchive** outArchive);

 private:
  Lnk4MetaEntry* TOC = 0;
};

}  // namespace Io
}  // namespace Impacto