#pragma once

#include "stream.h"
#include "buffering.h"
#include "../log.h"

namespace Impacto {
namespace Io {

int32_t LZXDecompress(uint8_t* CompressedBuffer, int CompressedSize,
                      uint8_t* UncompressedBuffer, int UncompressedSize,
                      int WindowSize, int CompressionPartitionSize);

class LzxStream : public Stream, public Buffering<LzxStream> {
  friend class Buffering<LzxStream>;

 public:
  ~LzxStream();

  bool IsSeekSlow = true;

  static IoError Create(Stream* baseStream, int64_t offset, int64_t size,
                        Stream** out);
  int64_t Read(void* buffer, int64_t sz) override;
  int64_t Seek(int64_t offset, int origin) override;
  IoError Duplicate(Stream** outStream) override;

 protected:
  LzxStream(int64_t uncompressedBufferSize)
      : Buffering(uncompressedBufferSize) {}
  LzxStream(LzxStream const& other) = default;

  IoError FillBuffer();

  Stream* BaseStream;
  int64_t CompressedOffset;
  int64_t CompressedPosition;
  int64_t CompressedSize;
  int64_t UncompressedPosition;
  int64_t UncompressedSize;

  int32_t WindowSize;
  int32_t CompressionPartitionSize;

  int32_t CompressedBufferSize;
  uint8_t* CompressedBuffer;
};

}  // namespace Io
}  // namespace Impacto