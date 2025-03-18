#pragma once

#include <cstdint>

namespace Impacto {

enum IoError : int64_t {
  IoError_OK = 0,
  IoError_Fail = -1,
  IoError_Eof = -2,
  IoError_NotFound = -3
};

constexpr inline auto format_as(IoError type) -> std::string_view {
  switch (type) {
    case IoError_OK:
      return "IoError_OK";
    case IoError_Fail:
      return "IoError_Fail";
    case IoError_Eof:
      return "IoError_Eof";
    case IoError_NotFound:
      return "IoError_NotFound";
  }
  return "Unknown";
}

}  // namespace Impacto