#pragma once

#include <vector>

#include <SDL_rwops.h>

#include "../impacto.h"
#include "io.h"

// only these for new vfs
#include "inputstream.h"
#include <flat_hash_map.hpp>

namespace Impacto {
namespace Io {

// For transitional purposes, I'm keeping *both* VFS implementations here for
// now.

// TODO:
// - Overlay
// - c0data style redirection (multiple source mountpoints -> one target)
// - Make the rest of the engine use the new VFS
// - Configurable physical file search paths
// - Invert search order, see below

// The public interface of vfs.h is threadsafe. Individual InputStreams are not.
// Duplicate() them if you need to use them on multiple threads.

static std::string const VfsArchiveBasePath = "./USRDIR";

void VfsInit();
// Mount an archive from a physical file.
// Files will always be loaded from the earliest-mounted archive they're found
// in
// TODO invert search order
IoError VfsMount(std::string const& mountpoint,
                 std::string const& archiveFileName);
// Mount an archive from memory. A unique filename must be specified to identify
// files coming from this archive and to unmount it.
IoError VfsMountMemory(std::string const& mountpoint,
                       std::string const& archiveFileName, void* memory,
                       int64_t size, bool freeOnClose);
// archiveFileName must match the filename an archive was mounted with
IoError VfsUnmount(std::string const& mountpoint,
                   std::string const& archiveFileName);
IoError VfsGetMeta(std::string const& mountpoint, std::string const& fileName,
                   FileMeta* outMeta);
IoError VfsGetMeta(std::string const& mountpoint, uint32_t id,
                   FileMeta* outMeta);
IoError VfsOpen(std::string const& mountpoint, std::string const& fileName,
                InputStream** outStream);
IoError VfsOpen(std::string const& mountpoint, uint32_t id,
                InputStream** outStream);
IoError VfsSlurp(std::string const& mountpoint, std::string const& fileName,
                 void** outMemory, int64_t* outSize);
IoError VfsSlurp(std::string const& mountpoint, uint32_t id, void** outMemory,
                 int64_t* outSize);
// You can provide a filled outListing, we'll clear it
IoError VfsListFiles(std::string const& mountpoint,
                     ska::flat_hash_map<uint32_t, std::string>& outListing);

class VfsArchive;
typedef IoError (*VfsArchiveFactory)(InputStream* stream,
                                     VfsArchive** outArchive);

bool VfsRegisterArchiver(VfsArchiveFactory f);

}  // namespace Io

// Warning: The old VFS is not threadsafe - within an archive, and error
// handling in general :(

// Including null terminator
const int VfsMaxPath = 224;

// The overlay FS allows replacing individual files in an archive using the
// physical filesystem for quick tweaks, e.g.
// <VfsOverlayPath>/MODEL.CPK/c002_010.cpk/0000
extern bool g_VfsOverlayEnabled;
// Path to root of overlay FS relative to working directory
extern char const g_VfsOverlayPath[];
// Path to look for any asset archives in relative to working directory
extern char const g_VfsBasePath[];

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

  // outResult == false => only Slurp supported;
  // return value != IoError_OK => file inaccessible
  IoError CanStream(uint32_t id, bool* outResult);
  IoError CanStream(const char* path, bool* outResult);

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
  virtual IoError DriverCanStream(uint32_t id, bool* outResult) = 0;
  // DriverSlurp() only needs to be implemented for files that can't be streamed
  virtual IoError DriverSlurp(uint32_t id, void* outBuffer) = 0;

 private:
  IoError OverlayOpen(uint32_t id, SDL_RWops** outHandle);
};

void VfsInit();

}  // namespace Impacto