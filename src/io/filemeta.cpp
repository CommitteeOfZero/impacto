

#include "filemeta.h"
#include "../log.h"
#include <system_error>

namespace Impacto {
namespace Io {

inline int64_t GetFileSize(std::string const& path) {
  std::error_code ec;
  uintmax_t result = std::filesystem::file_size(path, ec);
  if (ec) {
    ImpLog(LL_Error, LC_IO,
           "Error getting file size of file \"%s\", error: \"%s\"\n",
           path.c_str(), ec.message().c_str());
    return IoError_Fail;
  }
  // Hopefully no one has a file of size between int64_t max and uint64_t max
  return static_cast<int64_t>(result);
}

inline IoError PathExists(std::string const& path) {
  std::error_code ec;
  bool result = std::filesystem::exists(path, ec);
  if (ec) {
    ImpLog(LL_Error, LC_IO,
           "Error checking for file existence for file \"%s\", error: \"%s\"\n",
           path.c_str(), ec.message().c_str());
    return IoError_Fail;
  }
  return result == false ? IoError_NotFound : IoError_OK;
}

inline int8_t CreateDirectories(std::string const& path) {
  std::error_code ec;
  bool result = std::filesystem::create_directories(path, ec);
  if (ec) {
    ImpLog(LL_Error, LC_IO,
           "Error creating directories for file \"%s\", error: \"%s\"\n",
           path.c_str(), ec.message().c_str());
    return IoError_Fail;
  }
  return result;
}

inline IoError GetFilePermissions(std::string const& path,
                                  FilePermissionsFlags& flags) {
  std::error_code ec;
  flags = std::filesystem::status(path, ec).permissions();
  if (ec) {
    ImpLog(LL_Error, LC_IO,
           "Error retrieving permissions for file \"%s\", error: \"%s\"\n",
           path.c_str(), ec.message().c_str());
    return IoError_Fail;
  }
  return IoError_OK;
}

}  // namespace Io
}  // namespace Impacto