#pragma once

#include "vfs.h"

namespace Impacto {

// VFS drivers should be considered private by VfsArchive users.
// The VfsArchive functions handle things like overlay FS and child management.

struct VfsDriver {
  IoError (*Mount)(SDL_RWops* stream, VfsArchive** outArchive);
  // Unmount (without recursion) archive, including freeing the memory
  IoError (*Unmount)(VfsArchive* archive);
  IoError (*Open)(VfsArchive* archive, uint32_t id, SDL_RWops** outHandle);
  // outName must have space for VfsMaxPath characters (including null
  // terminator)
  IoError (*GetName)(VfsArchive* archive, uint32_t id, char* outName);
  IoError (*GetId)(VfsArchive* archive, const char* path, uint32_t* outId);
  IoError (*GetSize)(VfsArchive* archive, uint32_t id, size_t* outSize);
  // These return IoError_Eof if the last enumerated file is the last in the
  // archive
  IoError (*EnumerateStart)(VfsArchive* archive, uint32_t* outIterator,
                            VfsFileInfo* outFileInfo);
  IoError (*EnumerateNext)(VfsArchive* archive, uint32_t* inoutIterator,
                           VfsFileInfo* outFileInfo);
};

}  // namespace Impacto