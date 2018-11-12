#include "zlibstream.h"

namespace Impacto {
namespace Io {

ZlibStream::~ZlibStream() {
  free(InputBuffer);
  inflateEnd(&ZlibState);
}

IoError ZlibStream::Create(InputStream* baseStream, int64_t compressedOffset,
                           int64_t compressedSize, int64_t uncompressedSize,
                           InputStream** out) {
  if (compressedOffset + compressedSize > baseStream->Meta.Size)
    return IoError_Fail;
  InputStream* dup;
  int64_t err = baseStream->Duplicate(&dup);
  if (err != IoError_OK) return (IoError)err;
  err = dup->Seek(compressedOffset, RW_SEEK_SET);
  if (err != compressedOffset) {
    delete dup;
    return IoError_Fail;
  }
  z_stream zlibState;
  int zErr;
  if ((zErr = inflateInit(&zlibState)) != Z_OK) {
    delete dup;
    return IoError_Fail;
  }
  ZlibStream* result = new ZlibStream;
  result->BaseStream = dup;
  result->CompressedOffset = compressedOffset;
  result->CompressedSize = compressedSize;
  result->Meta.Size = uncompressedSize;
  result->ZlibState = zlibState;
  result->InputBuffer = (uint8_t*)malloc(ZlibStreamInputBufferSize);
  *out = (InputStream*)result;
  return IoError_OK;
}

int64_t ZlibStream::Read(void* buffer, int64_t sz) {
  return ReadBuffered(buffer, sz);
}

int64_t ZlibStream::Seek(int64_t offset, int origin) {
  int64_t absPos;
  if (origin == RW_SEEK_SET) {
    absPos = offset;
  } else if (origin == RW_SEEK_CUR) {
    absPos = Position + offset;
  } else if (origin == RW_SEEK_END) {
    absPos = Meta.Size - offset;
  }
  if (absPos < 0 || absPos > Meta.Size) return IoError_Fail;

  int64_t err = SeekBuffered(absPos);
  if (err < IoError_OK) {
    if (absPos < Position) {
      Position = 0;
      inflateReset(&ZlibState);
      ZlibState.avail_in = 0;
      ZlibState.next_in = 0;
      BaseStream->Seek(CompressedOffset, RW_SEEK_SET);
    }
    err = DiscardSeekBuffered(absPos);
  }
  if (err < IoError_OK) {
    return err;
  }
  return Position;
}

IoError ZlibStream::Duplicate(InputStream** outStream) {
  InputStream* dup;
  int64_t err = BaseStream->Duplicate(&dup);
  if (err != IoError_OK) return (IoError)err;
  err = dup->Seek(CompressedOffset, RW_SEEK_SET);
  if (err != CompressedOffset) {
    delete dup;
    return IoError_Fail;
  }
  z_stream zlibState;
  int zErr;
  if ((zErr = inflateInit(&zlibState)) != Z_OK) {
    delete dup;
    return IoError_Fail;
  }
  ZlibStream* result = new ZlibStream(*this);
  result->InputBuffer = (uint8_t*)malloc(ZlibStreamInputBufferSize);
  result->BaseStream = dup;
  result->ZlibState = zlibState;
  result->Position = 0;
  result->BufferFill = 0;
  result->BufferConsumed = 0;
  err = result->Seek(Position, RW_SEEK_SET);
  if (err != Position) {
    delete result;
    return IoError_Fail;
  }
  *outStream = (InputStream*)result;
  return IoError_OK;
}

IoError ZlibStream::FillBuffer() {
  int zErr;

  ZlibState.avail_out = BufferSize;
  ZlibState.next_out = Buffer;
  int64_t lastTotal = ZlibState.total_out;

  do {
    if (ZlibState.avail_in == 0) {
      int64_t read = BaseStream->Read(InputBuffer, ZlibStreamInputBufferSize);
      if (read < 0) return (IoError)read;
      if (read == 0) return IoError_Eof;
      ZlibState.next_in = InputBuffer;
      ZlibState.avail_in = read;
    }

    zErr = inflate(&ZlibState, Z_SYNC_FLUSH);
  } while (zErr == Z_OK && ZlibState.avail_out > 0);

  if (zErr != Z_OK && zErr != Z_STREAM_END) {
    return IoError_Fail;
  }

  BufferFill = ZlibState.total_out - lastTotal;
}

}  // namespace Io
}  // namespace Impacto