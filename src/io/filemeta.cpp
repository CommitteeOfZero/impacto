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
  const auto parentPath = Path(filePath).parent_path();
  if (createParent && parentPath.empty()) return IoError_OK;
  const auto dirPath = createParent ? parentPath : Path(filePath);
  bool result = std::filesystem::create_directories(dirPath, ec);
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

// User app config directory
std::string const& GetPlatformConfigDir() {
  static const std::string path = [] {
#if defined(__ANDROID__)  // prefer external storage dir for easier user access
    const char* configPath = SDL_AndroidGetExternalStoragePath();
    if (!configPath) {
      ImpLog(LogLevel::Fatal, LogChannel::IO,
             "Failed to get Android external storage path, error: {}\n",
             SDL_GetError());
      exit(1);
    }
    return configPath;
#elif defined(__unix__)  // prefer .config over .local/share
    std::filesystem::path result;
    const char* configPath = SDL_getenv("XDG_CONFIG_HOME");
    if (!configPath || *configPath == '\0') {
      configPath = SDL_getenv("HOME");
      if (!configPath || *configPath == '\0') {
        return std::string{};
      }
      result = std::filesystem::path(configPath) / ".config";
    } else {
      result = std::filesystem::path(configPath);
    }
    return (result / "impacto").string();
#else
    char* configPath = SDL_GetPrefPath("Committee of Zero", "Impacto");
    std::string result = configPath;
    SDL_free(configPath);
    return result;
#endif
  }();
  return path;
}

}  // namespace Io
}  // namespace Impacto