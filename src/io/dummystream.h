#pragma once

#include <stdexcept>
#include "stream.h"

namespace Impacto {
namespace Io {

class DummyStream : public Stream {
 public:
  static IoError Create(std::string const& fileName, Stream** out) {
    DummyStream* result = new DummyStream();
    result->Meta.FileName = fileName;
    *out = (Stream*)result;
    return IoError_OK;
  }
  int64_t Read(void* buffer, int64_t sz) override {
    assert(false && "DummyStream does not support IO operations!");
    throw(std::runtime_error("DummyStream does not support IO operations!"));
  }
  int64_t Seek(int64_t offset, int origin) override {
    assert(false && "DummyStream does not support IO operations!");
    throw(std::runtime_error("DummyStream does not support IO operations!"));
  }
  IoError Duplicate(Stream** outStream) override {
    auto* result = new DummyStream();
    result->Meta = this->Meta;
    *outStream = result;
    return IoError_OK;
  }
};
}  // namespace Io
}  // namespace Impacto