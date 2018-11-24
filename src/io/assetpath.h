#pragma once

#include "inputstream.h"

namespace Impacto {
namespace Io {

class AssetPath {
 public:
  std::string Mount = "";
  std::string FileName = "";
  uint32_t Id = 0;

  IoError Open(InputStream** outStream);
  IoError Slurp(void** outMemory, int64_t* outSize);
};

}  // namespace Io
}  // namespace Impacto