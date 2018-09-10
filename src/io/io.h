#pragma once

namespace Impacto {

enum IoError {
  IoError_OK = 0,
  IoError_NotFound,
  IoError_Eof,
  IoError_Busy,
  IoError_OutOfMemory,
  IoError_Fail
};
}