#include "assetpath.h"
#include "physicalfilestream.h"
#include "vfs.h"

namespace Impacto {
namespace Io {

IoError AssetPath::Open(InputStream** outStream) {
  if (Mount.empty()) {
    return PhysicalFileStream::Create(FileName, outStream);
  } else {
    if (FileName.empty()) {
      return VfsOpen(Mount, Id, outStream);
    } else {
      return VfsOpen(Mount, FileName, outStream);
    }
  }
}

IoError AssetPath::Slurp(void** outMemory, int64_t* outSize) {
  if (Mount.empty()) {
    InputStream* stream;
    IoError err = PhysicalFileStream::Create(FileName, &stream);
    if (err != IoError_OK) return err;
    *outMemory = malloc(stream->Meta.Size);
    *outSize = stream->Meta.Size;
    int64_t readErr = stream->Read(*outMemory, *outSize);
    // TODO should size output by Read() go into outSize, even though it may be
    // less than the allocated size?
    delete stream;
    if (readErr < 0) {
      free(*outMemory);
      err = IoError_Fail;
    } else {
      err = IoError_OK;
    }
    return err;
  } else {
    if (FileName.empty()) {
      return VfsSlurp(Mount, Id, outMemory, outSize);
    } else {
      return VfsSlurp(Mount, FileName, outMemory, outSize);
    }
  }
}

}  // namespace Io
}  // namespace Impacto