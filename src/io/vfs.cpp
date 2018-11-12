#include <vector>
#include <algorithm>

#include "vfs.h"
#include "mpkdriver.h"
#include "cpkdriver.h"

#include "../log.h"
#include "../util.h"

// Only these for new VFS
#include "vfs.h"
#include "../impacto.h"
#include <vector>
#include "vfsarchive.h"
#include "physicalfilestream.h"
#include "memorystream.h"
#include "../log.h"

namespace Impacto {
namespace Io {

static std::vector<VfsArchiveFactory> Archivers;
static ska::flat_hash_map<std::string, std::vector<VfsArchive*>> Mounts;
static SDL_mutex* Lock;

static std::string const VfsArchiveBasePath = "./USRDIR";

static IoError MountInternal(std::string const& mountpoint,
                             InputStream* stream) {
  VfsArchive* archive;
  IoError err = IoError_Fail;
  for (auto archiver : Archivers) {
    err = archiver(stream, &archive);
    if (err == IoError_OK) break;
  }
  if (err == IoError_OK) {
    archive->MountPoint = mountpoint;
    Mounts[mountpoint].push_back(archive);
  } else {
    ImpLog(LL_Error, LC_IO, "No archiver supports this file\n");
  }
  return err;
}

static VfsArchive* FindArchive(std::string const& mountpoint,
                               std::string const& fileName) {
  auto it = Mounts.find(mountpoint);
  if (it == Mounts.end()) return 0;
  for (auto archive : it->second) {
    if (archive->BaseStream->Meta.FileName == fileName) return archive;
  }
  return 0;
}

void VfsInit() { Lock = SDL_CreateMutex(); }

IoError VfsMount(std::string const& mountpoint,
                 std::string const& archiveFileName) {
  IoError err;
  SDL_LockMutex(Lock);

  ImpLog(LL_Debug, LC_IO, "Trying to mount \"%s\" on mountpoint \"%s\"\n",
         archiveFileName.c_str(), mountpoint.c_str());

  std::string physPath = VfsArchiveBasePath + "/" + archiveFileName;

  if (FindArchive(mountpoint, archiveFileName) != 0) {
    ImpLog(LL_Error, LC_IO, "File with this name already mounted!\n");
    err = IoError_Fail;
    goto end;
  }

  InputStream* archiveFile;
  err = PhysicalFileStream::Create(physPath, &archiveFile);
  if (err != IoError_OK) {
    ImpLog(LL_Debug, LC_IO, "Could not open physical file \"%s\"\n",
           physPath.c_str());
    goto end;
  }

  // PhysicalFileStream puts the full path in there, we want the path the user
  // specified
  archiveFile->Meta.FileName = archiveFileName;

  err = MountInternal(mountpoint, archiveFile);
  if (err != IoError_OK) {
    delete archiveFile;
  }

end:
  SDL_UnlockMutex(Lock);
  return err;
}

IoError VfsMountMemory(std::string const& mountpoint,
                       std::string const& archiveFileName, void* memory,
                       int64_t size, bool freeOnClose) {
  IoError err;
  SDL_LockMutex(Lock);

  ImpLog(
      LL_Debug, LC_IO,
      "Trying to mount in-memory archive named \"%s\" on mountpoint \"%s\"\n",
      archiveFileName.c_str(), mountpoint.c_str());

  if (FindArchive(mountpoint, archiveFileName) != 0) {
    err = IoError_Fail;
    if (freeOnClose) free(memory);
    goto end;
  }

  InputStream* archiveFile = new MemoryStream(memory, size, freeOnClose);
  archiveFile->Meta.FileName = archiveFileName;
  err = MountInternal(mountpoint, archiveFile);
  if (err != IoError_OK) {
    delete archiveFile;
  }

end:
  SDL_UnlockMutex(Lock);
  return err;
}

IoError VfsUnmount(std::string const& mountpoint,
                   std::string const& archiveFileName) {
  IoError err;
  SDL_LockMutex(Lock);

  ImpLog(LL_Debug, LC_IO,
         "Trying to unmount archive named \"%s\" on mountpoint \"%s\"\n",
         archiveFileName.c_str(), mountpoint.c_str());

  err = IoError_NotFound;
  auto it = Mounts.find(mountpoint);
  if (it == Mounts.end()) goto end;
  for (auto arcIt = it->second.begin(); arcIt != it->second.end(); arcIt++) {
    if ((*arcIt)->BaseStream->Meta.FileName == archiveFileName) {
      it->second.erase(arcIt);
      delete *arcIt;
      err = IoError_OK;
      break;
    }
  }
  if (err != IoError_OK) {
    ImpLog(LL_Debug, LC_IO, "Unmounting failed (file not mounted?)\n");
  }

end:
  SDL_UnlockMutex(Lock);
  return err;
}

static IoError GetOrigMetaInternal(std::string const& mountpoint,
                                   std::string const& fileName,
                                   FileMeta** outMeta,
                                   VfsArchive** outArchive) {
  auto it = Mounts.find(mountpoint);
  if (it == Mounts.end()) return IoError_NotFound;

  for (auto archive : it->second) {
    auto nameToId = archive->NamesToIds.find(fileName);
    if (nameToId != archive->NamesToIds.end()) {
      *outMeta = archive->IdsToFiles[nameToId->second];
      *outArchive = archive;
      return IoError_OK;
    }
  }
  return IoError_NotFound;
}

static IoError GetOrigMetaInternal(std::string const& mountpoint, uint32_t id,
                                   FileMeta** outMeta,
                                   VfsArchive** outArchive) {
  auto it = Mounts.find(mountpoint);
  if (it == Mounts.end()) return IoError_NotFound;

  for (auto archive : it->second) {
    auto idToFile = archive->IdsToFiles.find(id);
    if (idToFile != archive->IdsToFiles.end()) {
      *outMeta = idToFile->second;
      *outArchive = archive;
      return IoError_OK;
    }
  }
  return IoError_NotFound;
}

IoError VfsGetMeta(std::string const& mountpoint, std::string const& fileName,
                   FileMeta* outMeta) {
  IoError err;
  SDL_LockMutex(Lock);

  ImpLogSlow(LL_Debug, LC_IO,
             "Trying to get metadata for file \"%s\" on mountpoint \"%s\"\n",
             fileName.c_str(), mountpoint.c_str());

  FileMeta* origMeta;
  VfsArchive* archive;
  err = GetOrigMetaInternal(mountpoint, fileName, &origMeta, &archive);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "Could not get metadata\n");
    goto end;
  }

  *outMeta = *origMeta;
  outMeta->ArchiveMountPoint = mountpoint;
  outMeta->ArchiveFileName = archive->BaseStream->Meta.FileName;

end:
  SDL_UnlockMutex(Lock);
  return err;
}

IoError VfsGetMeta(std::string const& mountpoint, uint32_t id,
                   FileMeta* outMeta) {
  IoError err;
  SDL_LockMutex(Lock);

  ImpLogSlow(LL_Debug, LC_IO,
             "Trying to get metadata for file %d on mountpoint \"%s\"\n", id,
             mountpoint.c_str());

  FileMeta* origMeta;
  VfsArchive* archive;
  err = GetOrigMetaInternal(mountpoint, id, &origMeta, &archive);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "Could not get metadata\n");
    goto end;
  }

  *outMeta = *origMeta;
  outMeta->ArchiveMountPoint = mountpoint;
  outMeta->ArchiveFileName = archive->BaseStream->Meta.FileName;

end:
  SDL_UnlockMutex(Lock);
  return err;
}

static IoError OpenInternal(std::string const& mountpoint, VfsArchive* archive,
                            FileMeta* origMeta, InputStream** outStream) {
  IoError err;

  ImpLogSlow(
      LL_Debug, LC_IO,
      "Opening \"%s\" (%d) from mountpoint \"%s\" (archive file \"%s\")\n",
      origMeta->FileName.c_str(), origMeta->Id, mountpoint.c_str(),
      archive->BaseStream->Meta.FileName.c_str());

  err = archive->Open(origMeta, outStream);
  if (err != IoError_OK) {
    ImpLogSlow(LL_Debug, LC_IO, "VfsArchive->Open() failed, trying slurp\n");
    // maybe streaming is not supported
    void* memory;
    int64_t size;
    err = archive->Slurp(origMeta, &memory, &size);
    // TODO lazy slurp stream
    *outStream = new MemoryStream(memory, size, true);
  }
  if (err == IoError_OK) {
    (*outStream)->Meta.ArchiveFileName = archive->BaseStream->Meta.FileName;
    (*outStream)->Meta.ArchiveMountPoint = mountpoint;
    (*outStream)->Meta.FileName = origMeta->FileName;
  } else {
    ImpLog(LL_Error, LC_IO,
           "Opening \"%s\" (%d) from mountpoint \"%s\" (archive file \"%s\") "
           "failed\n",
           origMeta->FileName.c_str(), origMeta->Id, mountpoint.c_str(),
           archive->BaseStream->Meta.FileName.c_str());
  }
  return err;
}

IoError VfsOpen(std::string const& mountpoint, std::string const& fileName,
                InputStream** outStream) {
  IoError err;
  SDL_LockMutex(Lock);

  ImpLogSlow(LL_Debug, LC_IO,
             "Trying to open file \"%s\" on mountpoint \"%s\"\n",
             fileName.c_str(), mountpoint.c_str());

  FileMeta* origMeta;
  VfsArchive* archive;
  err = GetOrigMetaInternal(mountpoint, fileName, &origMeta, &archive);
  if (err != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "Could not get metadata\n");
    goto end;
  }

  err = OpenInternal(mountpoint, archive, origMeta, outStream);

end:
  SDL_UnlockMutex(Lock);
  return err;
}

IoError VfsOpen(std::string const& mountpoint, uint32_t id,
                InputStream** outStream) {
  IoError err;
  SDL_LockMutex(Lock);

  FileMeta* origMeta;
  VfsArchive* archive;
  err = GetOrigMetaInternal(mountpoint, id, &origMeta, &archive);
  if (err != IoError_OK) goto end;

  err = OpenInternal(mountpoint, archive, origMeta, outStream);

end:
  SDL_UnlockMutex(Lock);
  return err;
}

IoError SlurpInternal(VfsArchive* archive, FileMeta* origMeta, void** outMemory,
                      int64_t* outSize) {
  IoError err;

  ImpLogSlow(
      LL_Debug, LC_IO,
      "Slurping \"%s\" (%d) from mountpoint \"%s\" (archive file \"%s\")\n",
      origMeta->FileName.c_str(), origMeta->Id, archive->MountPoint.c_str(),
      archive->BaseStream->Meta.FileName.c_str());

  err = archive->Slurp(origMeta, outMemory, outSize);
  if (err != IoError_OK) {
    ImpLogSlow(LL_Debug, LC_IO, "VfsArchive->Slurp() failed, trying open\n");

    InputStream* stream;
    err = archive->Open(origMeta, &stream);
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
  }
  return err;
}

IoError VfsSlurp(std::string const& mountpoint, std::string const& fileName,
                 void** outMemory, int64_t* outSize) {
  IoError err;
  SDL_LockMutex(Lock);

  FileMeta* origMeta;
  VfsArchive* archive;
  err = GetOrigMetaInternal(mountpoint, fileName, &origMeta, &archive);
  if (err != IoError_OK) goto end;

  err = SlurpInternal(archive, origMeta, outMemory, outSize);

end:
  SDL_UnlockMutex(Lock);
  return err;
}

IoError VfsSlurp(std::string const& mountpoint, uint32_t id, void** outMemory,
                 int64_t* outSize) {
  IoError err;
  SDL_LockMutex(Lock);

  FileMeta* origMeta;
  VfsArchive* archive;
  err = GetOrigMetaInternal(mountpoint, id, &origMeta, &archive);
  if (err != IoError_OK) goto end;

  err = SlurpInternal(archive, origMeta, outMemory, outSize);

end:
  SDL_UnlockMutex(Lock);
  return err;
}

IoError VfsListFiles(std::string const& mountpoint,
                     ska::flat_hash_map<uint32_t, std::string>& outListing) {
  IoError err;
  SDL_LockMutex(Lock);

  auto it = Mounts.find(mountpoint);
  if (it == Mounts.end()) err = IoError_NotFound;

  outListing.clear();

  // Reverse order so things first in the search path get overwritten last
  for (auto arcIt = it->second.rbegin(); arcIt != it->second.rend(); arcIt++) {
    for (auto nameToId : (*arcIt)->NamesToIds) {
      outListing[nameToId.second] = nameToId.first;
    }
  }

  err = IoError_OK;

end:
  SDL_UnlockMutex(Lock);
  return err;
}

bool VfsRegisterArchiver(VfsArchiveFactory f) {
  Archivers.push_back(f);
  return true;
}

}  // namespace Io

bool g_VfsOverlayEnabled = true;
char const g_VfsOverlayPath[] = "./overlayfs";
char const g_VfsBasePath[] = "./USRDIR";

static std::vector<VfsMountProc> VfsDrivers;

struct LazySlurpStream : SDL_RWops {
  static IoError Create(VfsArchive* archive, uint32_t id, SDL_RWops** result) {
    ImpLog(LL_Error, LC_IO,
           "Creating LazySlurpStream for file %d in archive \"%s\"\n", id,
           archive->MountPoint);
    int64_t sz;
    IoError err = archive->GetSize(id, &sz);
    if (err != IoError_OK) {
      ImpLog(LL_Error, LC_IO, "LazySlurpStream creation failed");
      // File not readable
      return err;
    }
    if (sz > INT_MAX) {
      ImpLog(LL_Error, LC_IO,
             "File %d in archive \"%s\" over 2GB, too large for SDL memory "
             "stream\n",
             id, archive->MountPoint);
      return IoError_Fail;
    }
    *result = new LazySlurpStream(archive, id, sz);
    return IoError_OK;
  }

  ~LazySlurpStream() {
    if (IsOpen) {
      free(Memory);
    } else {
      Archive->OpenHandles--;
    }
  }

 private:
  LazySlurpStream(VfsArchive* archive, uint32_t id, int64_t fileSize)
      : Archive(archive), Id(id), FileSize(fileSize) {
    IsOpen = false;

    size = Size;
    seek = Seek;
    read = Read;
    write = Write;
    close = Close;
    type = SDL_RWOPS_UNKNOWN;

    Archive->OpenHandles++;
  }
  bool IsOpen;

  VfsArchive* Archive;
  uint32_t Id;
  int64_t FileSize;

  void* Memory;

  void Open() {
    assert(!IsOpen);

    ImpLog(LL_Debug, LC_IO,
           "Realizing LazySlurpStream for file %d in archive \"%s\"\n", Id,
           Archive->MountPoint);

    assert(Archive->Slurp(Id, &Memory, &FileSize) == IoError_OK);
    Archive->OpenHandles--;

    // Treat as an SDL memory stream from now, except for freeing resources
    SDL_RWops* tempStream = SDL_RWFromConstMem(Memory, FileSize);
    memcpy(this, tempStream, sizeof(LazySlurpStream));
    SDL_RWclose(tempStream);

    close = Close;
    type = SDL_RWOPS_UNKNOWN;
    IsOpen = true;
  }

  // Except Close, these functions will only be called when the stream has not
  // been opened; their entries in the function table will be overwritten
  // afterwards
  static int64_t SDLCALL Size(SDL_RWops* context) {
    return ((LazySlurpStream*)context)->FileSize;
  }
  static int64_t SDLCALL Seek(SDL_RWops* context, int64_t offset, int whence) {
    if (offset == 0 && whence == RW_SEEK_CUR) {
      // when unopened, position is always 0
      return 0;
    }
    ((LazySlurpStream*)context)->Open();
    return SDL_RWseek(context, offset, whence);
  }
  static size_t SDLCALL Read(SDL_RWops* context, void* ptr, size_t size,
                             size_t maxnum) {
    ((LazySlurpStream*)context)->Open();
    return SDL_RWread(context, ptr, size, maxnum);
  }
  static size_t SDLCALL Write(SDL_RWops* context, const void* ptr, size_t size,
                              size_t num) {
    char const error[] = "Tried to write to a LazySlurpStream";
    SDL_SetError(error);
    ImpLog(LL_Error, LC_IO, "%s\n", error);
    return 0;
  }
  static int SDLCALL Close(SDL_RWops* context) {
    delete (LazySlurpStream*)context;
    return 0;
  }
};

IoError VfsArchive::Mount(const char* archiveName, VfsArchive** outArchive) {
  IoError result = IoError_OK;

  ImpLog(LL_Debug, LC_IO, "Mounting archive \"%s\"\n", archiveName);

  if (strlen(archiveName) + 1 > VfsMaxPath) {
    ImpLog(LL_Error, LC_IO, "Archive name \"%s\" too long\n", archiveName);
    return IoError_NotFound;
  }

  size_t reqSz = snprintf(NULL, 0, "%s/%s", g_VfsBasePath, archiveName) + 1;

  if (reqSz > VfsMaxPath) {
    ImpLog(LL_Error, LC_IO, "Path \"%s/%s\" too long\n", g_VfsBasePath,
           archiveName);
    return IoError_NotFound;
  }

  char* fullPath = (char*)ImpStackAlloc(reqSz);
  sprintf(fullPath, "%s/%s", g_VfsBasePath, archiveName);

  SDL_RWops* stream = SDL_RWFromFile(fullPath, "rb");
  if (stream == NULL) {
    ImpLog(LL_Error, LC_IO, "Could not read file \"%s\": %s\n", fullPath,
           SDL_GetError());
    ImpStackFree(fullPath);
    return IoError_Fail;
  }

  ImpStackFree(fullPath);

  result = IoError_Fail;
  for (auto mount : VfsDrivers) {
    result = mount(stream, outArchive);
    if (result == IoError_OK) {
      break;
    } else {
      SDL_RWseek(stream, 0, SEEK_SET);
    }
  }

  if (result != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "No available driver for \"%s\"\n", fullPath);
    SDL_RWclose(stream);
    return IoError_Fail;
  }

  strcpy((*outArchive)->MountPoint, archiveName);
  (*outArchive)->BaseStream = stream;
  (*outArchive)->Mounted = true;

  return IoError_OK;
}

void VfsArchive::RegisterDriver(VfsMountProc driver) {
  VfsDrivers.push_back(driver);
}

IoError VfsArchive::MountChild(uint32_t id, VfsArchive** outArchive) {
  IoError result = IoError_OK;

  ImpLog(LL_Debug, LC_IO, "Mounting child archive %d in \"%s\"\n", id,
         MountPoint);

  char* childName = (char*)ImpStackAlloc(VfsMaxPath);
  result = GetName(id, childName);

  if (result != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "Could not mount child archive\n");
    ImpStackFree(childName);
    return result;
  }

  size_t nameReqSz = snprintf(NULL, 0, "%s/%s", MountPoint, childName) + 1;

  if (nameReqSz > VfsMaxPath) {
    ImpLog(LL_Error, LC_IO, "Path \"%s/%s\" too long\n", MountPoint, childName);
    ImpStackFree(childName);
    return IoError_NotFound;
  }

  int64_t size;
  void* data;
  result = Slurp(id, &data, &size);

  if (result != IoError_OK) {
    ImpStackFree(childName);
    return result;
  }

  SDL_RWops* stream = SDL_RWFromConstMem(data, size);

  result = IoError_Fail;
  for (auto mount : VfsDrivers) {
    result = mount(stream, outArchive);
    if (result == IoError_OK) {
      break;
    } else {
      SDL_RWseek(stream, 0, SEEK_SET);
    }
  }

  if (result != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "No available driver for \"%s/%s\"\n", MountPoint,
           childName);
    ImpStackFree(childName);
    SDL_RWclose(stream);
    free(data);
    return IoError_Fail;
  }

  sprintf((*outArchive)->MountPoint, "%s/%s", MountPoint, childName);
  ImpStackFree(childName);

  (*outArchive)->Parent = this;
  (*outArchive)->BaseStream = stream;
  (*outArchive)->InMemoryArchive = data;
  (*outArchive)->Mounted = true;

  Children.push_back(*outArchive);

  return IoError_OK;
}
IoError VfsArchive::MountChild(const char* path, VfsArchive** outArchive) {
  uint32_t id;
  IoError result = GetId(path, &id);
  if (result != IoError_OK) return result;
  return MountChild(id, outArchive);
}

VfsArchive::~VfsArchive() {
  // Let subclass factories delete in-progress archives
  if (Mounted) {
    ImpLog(LL_Debug, LC_IO, "Unmounting archive \"%s\"\n", MountPoint);

    for (auto child = Children.begin(); child != Children.end();) {
      // stop child from removing itself from our vector
      (*child)->Parent = NULL;
      delete *child;
      child = Children.erase(child);
    }

    if (OpenHandles > 0) {
      ImpLog(LL_Warning, LC_IO,
             "Unmounted archive \"%s\" while %d handles were still open\n",
             MountPoint, OpenHandles);
    }

    if (Parent != NULL) {
      auto parentIt = std::find(std::begin(Parent->Children),
                                std::end(Parent->Children), this);
      Parent->Children.erase(parentIt);
    }

    if (BaseStream != NULL) {
      SDL_RWclose(BaseStream);
    }

    if (InMemoryArchive != NULL) {
      free(InMemoryArchive);
    }
  }
}

IoError VfsArchive::Slurp(uint32_t id, void** outData, int64_t* outSize) {
  ImpLog(LL_Debug, LC_IO, "Slurping file %d in archive \"%s\"\n", id,
         MountPoint);

  IoError result = IoError_OK;

  bool canStream;
  result = CanStream(id, &canStream);
  if (result != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "CanStream lookup failed\n");
    return result;
  }

  result = GetSize(id, outSize);
  if (result != IoError_OK) return result;
  if (*outSize == 0) return IoError_OK;

  *outData = malloc(*outSize);
  if (*outData == NULL) return IoError_OutOfMemory;

  if (canStream) {
    SDL_RWops* file;
    result = Open(id, &file);
    if (result != IoError_OK) {
      free(*outData);
      return result;
    }

    size_t rwResult = SDL_RWread(file, *outData, 1, *outSize);
    if (rwResult == 0) {
      ImpLog(LL_Error, LC_IO, "Slurping file %d in archive \"%s\" failed: %s\n",
             SDL_GetError());
      free(*outData);
      result = IoError_Fail;
    }

    // Do this *after* potentially logging a SDL_GetError in case SDL_RWclose
    // changes last error
    SDL_RWclose(file);
  } else {
    result = DriverSlurp(id, *outData);
    if (result != IoError_OK) {
      ImpLog(LL_Error, LC_IO,
             "(Driver) Slurping file %d in archive \"%s\" failed: %s\n",
             SDL_GetError());
      free(*outData);
    }
  }

  return result;
}
IoError VfsArchive::Slurp(const char* path, void** outData, int64_t* outSize) {
  uint32_t id;
  IoError result = GetId(path, &id);
  if (result != IoError_OK) return result;
  return Slurp(id, outData, outSize);
}

IoError VfsArchive::Open(uint32_t id, SDL_RWops** outHandle) {
  IoError result = OverlayOpen(id, outHandle);
  if (result != IoError_OK) {
    bool canStream;
    result = CanStream(id, &canStream);
    if (result == IoError_OK) {
      if (canStream) {
        result = DriverOpen(id, outHandle);
      } else {
        result = LazySlurpStream::Create(this, id, outHandle);
      }
    }
  }
  return result;
}
IoError VfsArchive::Open(const char* path, SDL_RWops** outHandle) {
  uint32_t id;
  IoError result = GetId(path, &id);
  if (result != IoError_OK) return result;
  return Open(id, outHandle);
}

IoError VfsArchive::CanStream(uint32_t id, bool* outResult) {
  SDL_RWops* tmpHandle;
  IoError result = OverlayOpen(id, &tmpHandle);
  if (result == IoError_OK) {
    SDL_RWclose(tmpHandle);
    *outResult = true;
    return IoError_OK;
  } else {
    return DriverCanStream(id, outResult);
  }
}

IoError VfsArchive::CanStream(const char* path, bool* outResult) {
  uint32_t id;
  IoError result = GetId(path, &id);
  if (result != IoError_OK) return result;
  return CanStream(id, outResult);
}

IoError VfsArchive::GetSize(uint32_t id, int64_t* outSize) {
  SDL_RWops* handle;
  IoError result = OverlayOpen(id, &handle);
  if (result == IoError_OK) {
    *outSize = SDL_RWsize(handle);
    SDL_RWclose(handle);
    if (*outSize == -1) {
      ImpLog(LL_Error, LC_IO,
             "Invalid file size for %d in \"%s\" from overlay FS: %s\n", id,
             MountPoint, SDL_GetError());
      result = IoError_Fail;
    }
  } else {
    result = DriverGetSize(id, outSize);
  }

  return result;
}
IoError VfsArchive::GetSize(const char* path, int64_t* outSize) {
  uint32_t id;
  IoError result = GetId(path, &id);
  if (result != IoError_OK) return result;
  return GetSize(id, outSize);
}

IoError VfsArchive::OverlayOpen(uint32_t id, SDL_RWops** outHandle) {
  if (g_VfsOverlayEnabled) {
    ImpLog(LL_Debug, LC_IO, "Trying to open %d in \"%s\" with overlay FS\n", id,
           MountPoint);

    char fileName[VfsMaxPath];
    IoError overlayErr = GetName(id, fileName);
    if (overlayErr == IoError_OK) {
      size_t reqSz = snprintf(NULL, 0, "%s/%s/%s", g_VfsOverlayPath, MountPoint,
                              fileName) +
                     1;
      if (reqSz <= VfsMaxPath) {
        char* overlayPath = (char*)ImpStackAlloc(reqSz);

        sprintf(overlayPath, "%s/%s/%s", g_VfsOverlayPath, MountPoint,
                fileName);

        *outHandle = SDL_RWFromFile(overlayPath, "rb");

        ImpStackFree(overlayPath);

        if (*outHandle != NULL) {
          ImpLog(LL_Debug, LC_IO, "Opened %d in \"%s\" with overlay FS\n", id,
                 MountPoint);
          return IoError_OK;
        } else {
          ImpLog(LL_Debug, LC_IO, "No overlay file found for %d in \"%s\"\n",
                 id, MountPoint);
          return IoError_Fail;
        }

      } else {
        ImpLog(LL_Warning, LC_IO,
               "Cannot use overlay FS for \"%s/%s/%s\" - path too long\n",
               g_VfsOverlayPath, MountPoint, fileName);
        return IoError_Fail;
      }
    } else {
      return overlayErr;
    }
  } else {
    return IoError_Fail;
  }
}

void VfsInit() {
  MpkRegisterDriver();
  CpkRegisterDriver();
}

}  // namespace Impacto