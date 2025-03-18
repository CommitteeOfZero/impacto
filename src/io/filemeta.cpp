#include "filemeta.h"
#include "../log.h"
#include <system_error>

namespace Impacto {
namespace Io {

int64_t GetFileSize(std::string const& path) {
  const std::string& filePath = GetSystemDependentPath(path);
  std::error_code ec;
  uintmax_t result = std::filesystem::file_size(filePath, ec);
  if (ec) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Error getting file size of file \"{:s}\", error: \"{:s}\"\n", path,
           ec.message());
    return IoError_Fail;
  }
  // Hopefully no one has a file of size between int64_t max and uint64_t max
  return static_cast<int64_t>(result);
}

IoError PathExists(std::string const& path) {
  const std::string& filePath = GetSystemDependentPath(path);
  std::error_code ec;
  bool result = std::filesystem::exists(filePath, ec);
  if (ec) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Error checking for file existence for file \"{:s}\", error: "
           "\"{:s}\"\n",
           filePath, ec.message());
    return IoError_Fail;
  }
  return result == false ? IoError_NotFound : IoError_OK;
}

int8_t CreateDirectories(std::string const& path, bool createParent) {
  using Path = std::filesystem::path;
  std::error_code ec;
  const std::string& filePath = GetSystemDependentPath(path);
  bool result = std::filesystem::create_directories(
      (createParent) ? Path(filePath).parent_path() : Path(filePath), ec);
  if (ec) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Error creating directories for file \"{:s}\", error: \"{:s}\"\n",
           filePath, ec.message());
    return IoError_Fail;
  }
  return result;
}

IoError GetFilePermissions(std::string const& path,
                           FilePermissionsFlags& flags) {
  std::error_code ec;
  const std::string& filePath = GetSystemDependentPath(path);
  flags = std::filesystem::status(filePath, ec).permissions();
  if (ec) {
    ImpLog(LogLevel::Error, LogChannel::IO,
           "Error retrieving permissions for file \"{:s}\", error: \"{:s}\"\n",
           filePath, ec.message());
    return IoError_Fail;
  }
  return IoError_OK;
}

}  // namespace Io
}  // namespace Impacto