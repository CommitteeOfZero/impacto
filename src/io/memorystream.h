#pragma once

#include "inputstream.h"

namespace Impacto {
namespace Io {

class MemoryStream : public InputStream {
 public:
  ~MemoryStream();
  MemoryStream(void* mem, int64_t size, bool freeOnClose = false);

  int64_t Read(void* buffer, int64_t sz) override;
  int64_t Seek(int64_t offset, int origin) override;
  IoError Duplicate(InputStream** outStream) override;

 protected:
  MemoryStream() {}
  MemoryStream(MemoryStream const& other) = default;

  void* Memory;
  bool FreeOnClose;
};

}  // namespace Io
}  // namespace Impacto