#pragma once

#include "inputstream.h"
#include <algorithm>
#include <string.h>

namespace Impacto {
namespace Io {

template <class T>
class Buffering {
 protected:
  IoError FillBuffer();

  uint8_t* Buffer;
  int64_t BufferSize;
  int64_t BufferFill = 0;
  int64_t BufferConsumed = 0;

  Buffering(int64_t bufferSize) { Buffer = (uint8_t*)malloc(bufferSize); }
  Buffering(const Buffering& other)
      : BufferSize(other.BufferSize),
        BufferFill(other.BufferFill),
        BufferConsumed(other.BufferConsumed) {
    Buffer = (uint8_t*)malloc(BufferSize);
    memcpy(Buffer, other.Buffer, BufferSize);
  }
  ~Buffering() { free(Buffer); }

  int64_t ReadBuffered(void* buffer, int64_t sz) {
    T* stream = static_cast<T*>(this);
    if (sz < 0) return IoError_Fail;
    int64_t read = 0;
    if (stream->Position == stream->Meta.Size) return IoError_Eof;
    sz = std::min(stream->Meta.Size - stream->Position, sz);

    while (sz) {
      if (BufferConsumed < BufferFill) {
        int64_t toCopy = std::min(sz, BufferFill - BufferConsumed);
        if (buffer) {
          memcpy(buffer, Buffer + BufferConsumed, toCopy);
        }
        sz -= toCopy;
        stream->Position += toCopy;
        BufferConsumed += toCopy;
        read += toCopy;
        if (stream->Position == stream->Meta.Size) return read;
      } else {
        BufferConsumed = 0;
        BufferFill = 0;
        IoError err = stream->FillBuffer();
        if (err != IoError_OK) return err;
      }
    }
    return read;
  }

  int64_t SeekBuffered(int64_t pos) {
    T* stream = static_cast<T*>(this);
    if (stream->Position == pos) return pos;
    int64_t bufferStart = stream->Position - BufferConsumed;
    if (pos >= bufferStart && pos <= bufferStart + BufferFill) {
      stream->Position = pos;
      BufferConsumed = pos - bufferStart;
      return pos;
    } else {
      // TODO support streams that do have random access on their own
      return IoError_Fail;
    }
  }

  int64_t DiscardSeekBuffered(int64_t pos) {
    T* stream = static_cast<T*>(this);
    while (stream->Position < pos) {
      int64_t read = ReadBuffered(0, BufferSize);
      if (read < IoError_OK) return read;
    }
    return stream->SeekBuffered(pos);
  }
};

}  // namespace Io
}  // namespace Impacto