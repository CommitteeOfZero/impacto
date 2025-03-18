#pragma once

#include "stream.h"

namespace Impacto {
namespace Io {

class AssetPath {
 public:
  std::string Mount = "";
  std::string FileName = "";
  uint32_t Id = 0;

  IoError Open(Stream** outStream);
  IoError Slurp(void*& outMemory, int64_t& outSize);
};

}  // namespace Io
}  // namespace Impacto