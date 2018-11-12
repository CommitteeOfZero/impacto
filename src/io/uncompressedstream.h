#pragma once

#include "inputstream.h"

namespace Impacto {
namespace Io {

class UncompressedStream : public InputStream {
 public:
  ~UncompressedStream();

  static IoError Create(InputStream* baseStream, int64_t baseStreamOffset,
                        int64_t size, InputStream** out);
  int64_t Read(void* buffer, int64_t sz) override;
  int64_t Seek(int64_t offset, int origin) override;
  IoError Duplicate(InputStream** outStream) override;

 protected:
  UncompressedStream() {}
  UncompressedStream(UncompressedStream const& other) = default;

  InputStream* BaseStream;
  int64_t BaseStreamOffset;
};

}  // namespace Io
}  // namespace Impacto