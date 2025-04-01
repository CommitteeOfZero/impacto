#include "vfsarchive.h"

namespace Impacto {
namespace Io {

VfsArchive::~VfsArchive() {
  if (IsInit && BaseStream) delete BaseStream;
}

IoError VfsArchive::Slurp(FileMeta* file, void*& outBuffer, int64_t& outSize) {
  return IoError_Fail;
}

IoError VfsArchive::GetCurrentSize(FileMeta* file, int64_t& outSize) {
  outSize = file->Size;
  return IoError_OK;
}

}  // namespace Io
}  // namespace Impacto