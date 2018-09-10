#include <vector>
#include <algorithm>

#include "vfs.h"
#include "vfsdriver.h"

#include "../log.h"

namespace Impacto {

bool VfsOverlayEnabled = true;
const char VfsOverlayPath[] = "./overlayfs";
const char VfsBasePath[] = "./USRDIR";

static std::vector<VfsDriver*> VfsDrivers;

struct VfsArchive {
  // virtual folder including parents, e.g. "model.cpk/c002_010.cpk"
  char MountPoint[VfsMaxPath];

  SDL_RWops* BaseStream;
  // Used for child mounts, which are read into memory fully
  // and need to be freed on unmount
  void* InMemoryArchive;

  VfsDriver* Driver;

  VfsArchive* Parent;
  std::vector<VfsArchive*> Children;

  uint32_t OpenHandles;
};

void VfsRegisterDriver(VfsDriver* driver) { VfsDrivers.push_back(driver); }

IoError VfsMount(const char* archiveName, VfsArchive** outArchive) {
  IoError result = IoError_OK;

  ImpLog(LL_Debug, LC_IO, "Mounting archive \"%s\"\n", archiveName);

  if (strlen(archiveName) + 1 > VfsMaxPath) {
    ImpLog(LL_Error, LC_IO, "Archive name \"%s\" too long\n", archiveName);
    return IoError_NotFound;
  }

  size_t reqSz = snprintf(NULL, 0, "%s/%s", VfsBasePath, archiveName) + 1;

  if (reqSz > VfsMaxPath) {
    ImpLog(LL_Error, LC_IO, "Path \"%s/%s\" too long\n", VfsBasePath,
           archiveName);
    return IoError_NotFound;
  }

  char* fullPath = (char*)ImpStackAlloc(reqSz);
  sprintf(fullPath, "%s/%s", VfsBasePath, archiveName);

  SDL_RWops* stream = SDL_RWFromFile(fullPath, "rb");
  if (stream == NULL) {
    ImpLog(LL_Error, LC_IO, "Could not read file \"%s\": %s\n", fullPath,
           SDL_GetError());
    ImpStackFree(fullPath);
    return IoError_Fail;
  }

  ImpStackFree(fullPath);

  result = IoError_Fail;
  for (auto driver : VfsDrivers) {
    result = driver->Mount(stream, outArchive);
    if (result == IoError_OK) break;
  }

  if (result != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "No available driver for \"%s\"\n", fullPath);
    SDL_RWclose(stream);
    return IoError_Fail;
  }

  strcpy((*outArchive)->MountPoint, archiveName);
  (*outArchive)->BaseStream = stream;

  return IoError_OK;
}

IoError VfsMountChild(VfsArchive* parent, uint32_t id,
                      VfsArchive** outArchive) {
  IoError result = IoError_OK;

  ImpLog(LL_Debug, LC_IO, "Mounting child archive %d in \"%s\"\n", id,
         parent->MountPoint);

  char* childName = (char*)ImpStackAlloc(VfsMaxPath);
  result = VfsGetName(parent, id, childName);

  if (result != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "Could not mount child archive\n");
    ImpStackFree(childName);
    return result;
  }

  size_t nameReqSz =
      snprintf(NULL, 0, "%s/%s", parent->MountPoint, childName) + 1;

  if (nameReqSz > VfsMaxPath) {
    ImpLog(LL_Error, LC_IO, "Path \"%s/%s\" too long\n", parent->MountPoint,
           childName);
    ImpStackFree(childName);
    return IoError_NotFound;
  }

  size_t size;
  void* data;
  result = VfsSlurp(parent, id, &data, &size);

  if (result != IoError_OK) {
    ImpStackFree(childName);
    return result;
  }

  SDL_RWops* stream = SDL_RWFromConstMem(data, size);

  result = IoError_Fail;
  for (auto driver : VfsDrivers) {
    result = driver->Mount(stream, outArchive);
    if (result == IoError_OK) break;
  }

  if (result != IoError_OK) {
    ImpLog(LL_Error, LC_IO, "No available driver for \"%s/%s\"\n",
           parent->MountPoint, childName);
    ImpStackFree(childName);
    SDL_RWclose(stream);
    free(data);
    return IoError_Fail;
  }

  sprintf((*outArchive)->MountPoint, "%s/%s", parent->MountPoint, childName);
  ImpStackFree(childName);

  (*outArchive)->Parent = parent;
  (*outArchive)->BaseStream = stream;
  (*outArchive)->InMemoryArchive = data;

  return IoError_OK;
}
IoError VfsMountChild(VfsArchive* parent, const char* path,
                      VfsArchive** outArchive) {
  uint32_t id;
  IoError result = VfsGetId(parent, path, &id);
  if (result != IoError_OK) return result;
  return VfsMountChild(parent, id, outArchive);
}

IoError VfsUnmount(VfsArchive* archive) {
  ImpLog(LL_Debug, LC_IO, "Unmounting archive \"%s\"\n", archive->MountPoint);

  IoError result = IoError_OK;

  for (auto child = archive->Children.begin();
       child != archive->Children.end();) {
    // stop child from removing itself from our vector
    (*child)->Parent = NULL;
    IoError childResult = VfsUnmount(*child);
    if (childResult == IoError_OK) {
      child = archive->Children.erase(child);
    } else {
      result = childResult;
      (*child)->Parent = archive;
    }
  }

  if (result != IoError_OK) {
    ImpLog(LL_Error, LC_IO,
           "Child failed to unmount while unmounting archive \"%s\"\n",
           archive->MountPoint);
    return IoError_Busy;
  }

  if (archive->OpenHandles > 0) {
    ImpLog(LL_Error, LC_IO,
           "Tried to unmount archive \"%s\" while %d handles were still open\n",
           archive->MountPoint, archive->OpenHandles);
    return IoError_Busy;
  }

  void* mem = archive->InMemoryArchive;
  std::vector<VfsArchive*>::iterator parentIt;
  VfsArchive* parent = archive->Parent;
  bool removeFromParent = parent != NULL;
  if (removeFromParent) {
    parentIt = std::find(std::begin(parent->Children),
                         std::end(parent->Children), archive);
  }

  result = archive->Driver->Unmount(archive);

  if (result == IoError_OK) {
    if (removeFromParent) parent->Children.erase(parentIt);
    if (mem) free(mem);
  }

  return result;
}

IoError VfsSlurp(VfsArchive* archive, uint32_t id, void** outData,
                 size_t* outSize) {
  ImpLog(LL_Debug, LC_IO, "Slurping file %d in archive \"%s\"\n", id,
         archive->MountPoint);

  IoError result = IoError_OK;

  result = VfsGetSize(archive, id, outSize);
  if (result != IoError_OK) return result;
  if (*outSize == NULL) return IoError_OK;

  *outData = malloc(*outSize);
  if (*outData == NULL) return IoError_OutOfMemory;

  SDL_RWops* file;
  result = VfsOpen(archive, id, &file);
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

  return result;
}
IoError VfsSlurp(VfsArchive* archive, const char* path, void** outData,
                 size_t* outSize) {
  uint32_t id;
  IoError result = VfsGetId(archive, path, &id);
  if (result != IoError_OK) return result;
  return VfsSlurp(archive, id, outData, outSize);
}

IoError VfsOpen(VfsArchive* archive, uint32_t id, SDL_RWops** outHandle) {
  if (VfsOverlayEnabled) {
    // TODO implement
  }

  return archive->Driver->Open(archive, id, outHandle);
}
IoError VfsOpen(VfsArchive* archive, const char* path, SDL_RWops** outHandle) {
  uint32_t id;
  IoError result = VfsGetId(archive, path, &id);
  if (result != IoError_OK) return result;
  return VfsOpen(archive, id, outHandle);
}

IoError VfsGetName(VfsArchive* archive, uint32_t id, char* outName) {
  return archive->Driver->GetName(archive, id, outName);
}
IoError VfsGetId(VfsArchive* archive, const char* path, uint32_t* outId) {
  return archive->Driver->GetId(archive, path, outId);
}

IoError VfsGetSize(VfsArchive* archive, uint32_t id, size_t* outSize) {
  if (VfsOverlayEnabled) {
    // TODO implement
  }

  return archive->Driver->GetSize(archive, id, outSize);
}
IoError VfsGetSize(VfsArchive* archive, const char* path, size_t* outSize) {
  uint32_t id;
  IoError result = VfsGetId(archive, path, &id);
  if (result != IoError_OK) return result;
  return VfsGetSize(archive, id, outSize);
}

IoError VfsEnumerateStart(VfsArchive* archive, uint32_t* outIterator,
                          VfsFileInfo* outFileInfo) {
  return archive->Driver->EnumerateStart(archive, outIterator, outFileInfo);
}
IoError VfsEnumerateNext(VfsArchive* archive, uint32_t* inoutIterator,
                         VfsFileInfo* outFileInfo) {
  return archive->Driver->EnumerateNext(archive, inoutIterator, outFileInfo);
}

}  // namespace Impacto