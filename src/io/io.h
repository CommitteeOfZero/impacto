#pragma once

#include <cstdint>

namespace Impacto {

enum IoError : int64_t {
  IoError_OK = 0,
  IoError_Fail = -1,
  IoError_Eof = -2,
  IoError_NotFound = -3
};

}  // namespace Impacto