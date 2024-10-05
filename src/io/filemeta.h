#pragma once

#include <string>
#include <cstdint>
#include <filesystem>
#include "io.h"
namespace Impacto {
namespace Io {

struct FileMeta {
  std::string ArchiveFileName = "";
  std::string ArchiveMountPoint = "";
  std::string FileName = "";
  uint32_t Id = 0;
  int64_t Size = 0;
};

using FilePermissionsFlags = std::filesystem::perms;

int64_t GetFileSize(std::string const& path);
IoError PathExists(std::string const& path);
int8_t CreateDirectories(std::string const& path);
IoError GetFilePermissions(std::string const& path,
                           FilePermissionsFlags& flags);

}  // namespace Io
}  // namespace Impacto