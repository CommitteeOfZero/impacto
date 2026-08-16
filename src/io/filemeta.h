#pragma once

#include <string>
#include <cstdint>
#include <filesystem>
#include "io.h"

#ifdef __ANDROID__
#include <SDL3/SDL_system.h>
// Safe to bind to const ref, since temporaries have their lifetimes extended
#define GetSystemDependentPath(filePath)                                   \
  ((filePath.rfind(SDL_GetAndroidExternalStoragePath(), 0) ==              \
    std::string::npos)                                                     \
       ? SDL_GetAndroidExternalStoragePath() + std::string("/") + filePath \
       : filePath)
#else
#define GetSystemDependentPath(filePath) filePath
#endif

namespace Impacto {
namespace Io {

struct FileMeta {
  std::string ArchiveFileName = "";
  std::string ArchiveMountPoint = "";
  std::string FileName = "";
  uint32_t Id = 0;
  int64_t Size = 0;
};

typedef enum { Set, Cur, End } IoSeek;

// TODO: use our own perms enum
using FilePermissionsFlags = std::filesystem::perms;

int64_t GetFileSize(std::string const& path);
IoError PathExists(std::string const& path);
int8_t CreateDirectories(std::string const& path, bool createParent = false);
IoError GetFilePermissions(std::string const& path,
                           FilePermissionsFlags& flags);

std::string const& GetPlatformConfigDir();
}  // namespace Io
}  // namespace Impacto