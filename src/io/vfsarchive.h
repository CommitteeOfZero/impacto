#pragma once

#include "inputstream.h"
#include <flat_hash_map.hpp>

namespace Impacto {
namespace Io {

class VfsArchive {
 public:
  virtual ~VfsArchive() {
    if (IsInit && BaseStream) delete BaseStream;
  }

  // Meta.ArchiveFileName, Meta.ArchiveMountPoint, Meta.FileName are set by VFS,
  // not by the archiver.
  // These methods are only ever called with FileMeta* found in IdsToFiles.
  virtual IoError Open(FileMeta* file, InputStream** outStream) = 0;
  virtual IoError Slurp(FileMeta* file, void** outBuffer, int64_t* outSize) = 0;

  ska::flat_hash_map<std::string, uint32_t> NamesToIds;
  ska::flat_hash_map<uint32_t, FileMeta*> IdsToFiles;

  std::string MountPoint;

  bool IsInit = false;
  InputStream* BaseStream = 0;
};

}  // namespace Io
}  // namespace Impacto