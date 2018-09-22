#include <vector>
#include <algorithm>

#include "vfs.h"
#include "mpkdriver.h"

#include "../log.h"
#include "../util.h"

namespace Impacto {

bool g_VfsOverlayEnabled = true;
char const g_VfsOverlayPath[] = "./overlayfs";
char const g_VfsBasePath[] = "./USRDIR";

static std::vector<VfsMountProc> VfsDrivers;

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
  if (*outSize == NULL) return IoError_OK;

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
    result = DriverOpen(id, outHandle);
  }
  if (result == IoError_OK) {
    OpenHandles++;
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
    }
  } else {
    return IoError_Fail;
  }
}

void VfsInit() { MpkRegisterDriver(); }

}  // namespace Impacto