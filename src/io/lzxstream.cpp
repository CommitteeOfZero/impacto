#include "lzxstream.h"

#include "system.h"
#include "lzx.h"

namespace Impacto {
namespace Io {

LzxStream::~LzxStream() { free(CompressedBuffer); }

IoError LzxStream::Create(InputStream *baseStream, int64_t offset, int64_t size,
                          InputStream **out) {
  if (offset + size > baseStream->Meta.Size) return IoError_Fail;
  InputStream *dup;
  int64_t err = baseStream->Duplicate(&dup);
  if (err != IoError_OK) return (IoError)err;
  err = dup->Seek(offset, RW_SEEK_SET);
  if (err != offset) {
    delete dup;
    return IoError_Fail;
  }

  uint32_t const magic = 0x0FF512EE;

  if (ReadBE<uint32_t>(dup) != magic) {
    ImpLog(LL_Trace, LC_IO, "Not LZX\n");
    delete dup;
    return IoError_Fail;
  }

  err = dup->Seek(12, RW_SEEK_CUR);
  if (err < IoError_OK) {
    delete dup;
    return (IoError)err;
  }

  int32_t windowSize = ReadBE<int32_t>(dup);
  int32_t compressionPartitionSize = ReadBE<int32_t>(dup);

  int64_t uncompressedSize = ReadBE<int64_t>(dup);
  int64_t compressedSize = ReadBE<int64_t>(dup);

  int32_t uncompressedBlockSize = ReadBE<int32_t>(dup);
  int32_t compressedBlockSize = ReadBE<int32_t>(dup);

  int32_t compressedOffset = offset + 48;

  if (compressedSize + 48 > size) {
    delete dup;
    return IoError_Fail;
  }

  LzxStream *result = new LzxStream(uncompressedBlockSize);
  result->BaseStream = dup;
  result->CompressedOffset = compressedOffset;
  result->CompressedPosition = 0;
  result->CompressedSize = compressedSize;
  result->UncompressedPosition = 0;
  result->UncompressedSize = uncompressedSize;
  result->Meta.Size = uncompressedSize;

  result->WindowSize = windowSize;
  result->CompressionPartitionSize = compressionPartitionSize;
  result->CompressedBuffer = (uint8_t *)malloc(compressedBlockSize);
  result->CompressedBufferSize = compressedBlockSize;
  *out = (InputStream *)result;
  return IoError_OK;
}

int64_t LzxStream::Read(void *buffer, int64_t sz) {
  return ReadBuffered(buffer, sz);
}

int64_t LzxStream::Seek(int64_t offset, int origin) {
  int64_t absPos;
  switch (origin) {
    case RW_SEEK_SET:
      absPos = offset;
      break;
    case RW_SEEK_CUR:
      absPos = Position + offset;
      break;
    case RW_SEEK_END:
      absPos = Meta.Size - offset;
      break;
  }
  if (absPos < 0 || absPos > Meta.Size) return IoError_Fail;

  int64_t err = SeekBuffered(absPos);
  if (err < IoError_OK) {
    if (absPos < Position) {
      Position = 0;
      CompressedPosition = 0;
      UncompressedPosition = 0;
      BaseStream->Seek(CompressedOffset, RW_SEEK_SET);
    }
    err = DiscardSeekBuffered(absPos);
  }
  if (err < IoError_OK) {
    return err;
  }
  return Position;
}

IoError LzxStream::Duplicate(InputStream **outStream) {
  InputStream *dup;
  int64_t err = BaseStream->Duplicate(&dup);
  if (err != IoError_OK) return (IoError)err;
  LzxStream *result = new LzxStream(*this);
  result->CompressedBuffer = (uint8_t *)malloc(CompressedBufferSize);
  result->BaseStream = dup;
  *outStream = (InputStream *)result;
  return IoError_OK;
}

IoError LzxStream::FillBuffer() {
  uint32_t compressedBytes = ReadBE<uint32_t>(BaseStream);
  if (compressedBytes > static_cast<uint32_t>(CompressedBufferSize)) {
    // error: LZX block size larger than advertised
    return IoError_Fail;
  }
  if (CompressedPosition + compressedBytes > CompressedSize) {
    // error: LZX block out of file bounds
    return IoError_Fail;
  }
  CompressedPosition += compressedBytes;

  uint32_t bufPos = 0;
  while (bufPos < compressedBytes) {
    uint32_t read =
        BaseStream->Read(CompressedBuffer, compressedBytes - bufPos);
    if (read <= 0) return IoError_Fail;
    bufPos += read;
  }

  int64_t remaining_bytes = UncompressedSize - UncompressedPosition;
  int32_t expected_bytes = BufferSize;
  if (BufferSize > remaining_bytes) expected_bytes = remaining_bytes;
  int32_t result =
      LZXDecompress(CompressedBuffer, compressedBytes, Buffer, expected_bytes,
                    WindowSize, CompressionPartitionSize);
  if (result < 0) return IoError_Fail;
  UncompressedPosition += result;

  BufferFill = result;
  return IoError_OK;
}

struct LZXFile {
  uint8_t *buf;
  int bufSize;
  int pos;
  int rest;
};

// Based on
// https://github.com/gildor2/UEViewer/blob/master/Unreal/UnCoreCompression.cpp

static int LZXread(struct LZXFile *file, void *buffer, int bytes) {
  if (!file->rest) {
    // read block header
    if (file->buf[file->pos] == 0xFF) {
      // [0]   = FF
      // [1,2] = uncompressed block size
      // [3,4] = compressed block size
      file->rest = (file->buf[file->pos + 3] << 8) | file->buf[file->pos + 4];
      file->pos += 5;
    } else {
      // [0,1] = compressed size
      file->rest = (file->buf[file->pos + 0] << 8) | file->buf[file->pos + 1];
      file->pos += 2;
    }
    if (file->rest > file->bufSize - file->pos)
      file->rest = file->bufSize - file->pos;
  }
  if (bytes > file->rest) bytes = file->rest;
  if (!bytes) return 0;

  // copy block data
  memcpy(buffer, file->buf + file->pos, bytes);
  file->pos += bytes;
  file->rest -= bytes;

  return bytes;
}

static int LZXwrite(struct LZXFile *file, void *buffer, int bytes) {
  memcpy(file->buf + file->pos, buffer, bytes);
  file->pos += bytes;
  return bytes;
}

static void *LZXalloc(struct mspack_system *self, size_t bytes) {
  return malloc(bytes);
}

static void LZXfree(void *ptr) { free(ptr); }

static void LZXCopy(void *src, void *dst, size_t bytes) {
  memcpy(dst, src, bytes);
}

static struct mspack_system LZXSys = {
    NULL,  // open
    NULL,  // close
    (int (*)(mspack_file *, void *, int)) & LZXread,
    (int (*)(mspack_file *, void *, int)) & LZXwrite,
    NULL,  // seek
    NULL,  // tell
    NULL,  // message
    &LZXalloc,
    &LZXfree,
    &LZXCopy,
    NULL};

int32_t LZXDecompress(uint8_t *CompressedBuffer, int CompressedSize,
                      uint8_t *UncompressedBuffer, int UncompressedSize,
                      int WindowSize, int CompressionPartitionSize) {
  // setup streams
  struct LZXFile src, dst;
  src.buf = CompressedBuffer;
  src.bufSize = CompressedSize;
  src.pos = 0;
  src.rest = 0;
  dst.buf = UncompressedBuffer;
  dst.bufSize = UncompressedSize;
  dst.pos = 0;
  // prepare decompressor
  if (WindowSize >= 32) {  // max is 21 or 25 (delta)
    int t = WindowSize;
    for (WindowSize = 0; t >>= 1; WindowSize++)
      ;
  }
  if (WindowSize <= 0) WindowSize = 17;
  if (CompressionPartitionSize <= 0) CompressionPartitionSize = 256 * 1024;
  struct lzxd_stream *lzxd =
      lzxd_init(&LZXSys, (mspack_file *)&src, (mspack_file *)&dst, WindowSize,
                0, CompressionPartitionSize, UncompressedSize, 0);
  // decompress
  int r = lzxd_decompress(lzxd, UncompressedSize);
  if (r != MSPACK_ERR_OK) return -1;
  int32_t ret = dst.pos;
  // free resources
  lzxd_free(lzxd);

  return ret;
}

}  // namespace Io
}  // namespace Impacto