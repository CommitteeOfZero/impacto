#pragma once

#include "stream.h"

namespace Impacto {
namespace Io {

class UncompressedStream : public Stream {
 public:
  ~UncompressedStream();

  static IoError Create(Stream* baseStream, int64_t baseStreamOffset,
                        int64_t size, Stream** out);
  int64_t Read(void* buffer, int64_t sz) override;
  int64_t Seek(int64_t offset, int origin) override;
  IoError Duplicate(Stream** outStream) override;

 protected:
  UncompressedStream() {}
  UncompressedStream(UncompressedStream const& other) = default;

  Stream* BaseStream;
  int64_t BaseStreamOffset;
};

}  // namespace Io
}  // namespace Impacto