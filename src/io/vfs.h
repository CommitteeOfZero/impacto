#pragma once

#include <vector>

#include <SDL_rwops.h>

#include "../impacto.h"
#include "io.h"

// Warning: The VFS is not threadsafe - within an archive, and error handling in
// general :(

namespace Impacto {

// Including null terminator
const int VfsMaxPath = 224;

// The overlay FS allows replacing individual files in an archive using the
// physical filesystem for quick tweaks, e.g.
// <VfsOverlayPath>/MODEL.CPK/c002_010.cpk/0000
extern bool VfsOverlayEnabled;
// Path to root of overlay FS relative to working directory
extern const char VfsOverlayPath[];
// Path to look for any asset archives in relative to working directory
extern const char VfsBasePath[];

struct VfsFileInfo {
  uint32_t Id;
  char Name[VfsMaxPath];
};

class VfsArchive;

typedef IoError (*VfsMountProc)(SDL_RWops* stream, VfsArchive** outArchive);

class VfsArchive {
 public:
  static IoError Mount(const char* archiveName, VfsArchive** outArchive);
  static void RegisterDriver(VfsMountProc driver);

  // Entire child archive is loaded into memory
  IoError MountChild(uint32_t id, VfsArchive** outArchive);
  IoError MountChild(const char* path, VfsArchive** outArchive);

  // Reads whole file into malloc()'d buffer
  // Data must be freed by caller if no error occurs
  IoError Slurp(uint32_t id, void** outData, int64_t* outSize);
  IoError Slurp(const char* path, void** outData, int64_t* outSize);

  IoError Open(uint32_t id, SDL_RWops** outHandle);
  IoError Open(const char* path, SDL_RWops** outHandle);

  // outName must have space for VfsMaxPath characters (including null
  // terminator)
  virtual IoError GetName(uint32_t id, char* outName) = 0;
  virtual IoError GetId(const char* path, uint32_t* outId) = 0;

  virtual IoError EnumerateStart(uint32_t* outIterator,
                                 VfsFileInfo* outFileInfo) = 0;
  virtual IoError EnumerateNext(uint32_t* inoutIterator,
                                VfsFileInfo* outFileInfo) = 0;

  IoError GetSize(uint32_t id, int64_t* outSize);
  IoError GetSize(const char* path, int64_t* outSize);

  virtual ~VfsArchive();

  // These are for use by driver file-handle functions
 public:
  // virtual folder including parents, e.g. "model.cpk/c002_010.cpk"
  char MountPoint[VfsMaxPath] = {0};
  SDL_RWops* BaseStream = NULL;
  uint32_t OpenHandles = 0;

 protected:
  // Used for child mounts, which are read into memory fully
  // and need to be freed on unmount
  void* InMemoryArchive = NULL;

  VfsArchive* Parent = NULL;
  std::vector<VfsArchive*> Children;

  bool Mounted = false;

 protected:
  virtual IoError DriverOpen(uint32_t id, SDL_RWops** outHandle) = 0;
  virtual IoError DriverGetSize(uint32_t id, int64_t* outSize) = 0;

 private:
  IoError OverlayOpen(uint32_t id, SDL_RWops** outHandle);
};

}  // namespace Impacto