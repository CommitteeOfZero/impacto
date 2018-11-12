#pragma once

#include <string>

namespace Impacto {
namespace Io {

struct FileMeta {
  std::string ArchiveFileName = "";
  std::string ArchiveMountPoint = "";
  std::string FileName = "";
  uint32_t Id = 0;
  int64_t Size = 0;
};

}  // namespace Io
}  // namespace Impacto