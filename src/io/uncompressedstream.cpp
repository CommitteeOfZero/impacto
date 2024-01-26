#include "uncompressedstream.h"

#include <algorithm>

namespace Impacto {
namespace Io {

UncompressedStream::~UncompressedStream() {
  if (FreeBase) delete BaseStream;
}

IoError UncompressedStream::Create(InputStream* baseStream,
                                   int64_t baseStreamOffset, int64_t size,
                                   InputStream** out, bool freeBase) {
  if (baseStreamOffset + size > baseStream->Meta.Size) return IoError_Fail;
  InputStream* dup;
  int64_t err = baseStream->Duplicate(&dup);
  if (err != IoError_OK) return (IoError)err;
  err = dup->Seek(baseStreamOffset, RW_SEEK_SET);
  if (err < 0) {
    delete dup;
    return (IoError)err;
  }
  UncompressedStream* result = new UncompressedStream;
  result->BaseStream = dup;
  result->BaseStreamOffset = baseStreamOffset;
  result->Meta.Size = size;
  result->FreeBase = freeBase;
  *out = (InputStream*)result;
  return IoError_OK;
}

int64_t UncompressedStream::Read(void* buffer, int64_t sz) {
  if (sz < 0) return IoError_Fail;
  if (Position == Meta.Size) return IoError_Eof;
  sz = std::min(Meta.Size - Position, sz);
  int64_t read = BaseStream->Read(buffer, sz);
  if (read < 0) return read;
  Position += read;
  return read;
}

int64_t UncompressedStream::Seek(int64_t offset, int origin) {
  int64_t newPosInBase = IoError_Fail;
  if (origin == RW_SEEK_SET && (offset >= 0 && offset <= Meta.Size)) {
    newPosInBase = BaseStream->Seek(BaseStreamOffset + offset, RW_SEEK_SET);
  } else if (origin == RW_SEEK_CUR &&
             (Position + offset >= 0 && Position + offset <= Meta.Size)) {
    newPosInBase = BaseStream->Seek(offset, RW_SEEK_CUR);
  } else if (origin == RW_SEEK_END && (offset >= 0 && offset <= Meta.Size)) {
    int64_t end = BaseStreamOffset + Meta.Size;
    int64_t ourEndFromBaseEnd = BaseStream->Meta.Size - end;
    newPosInBase = BaseStream->Seek(ourEndFromBaseEnd + offset, RW_SEEK_END);
  }
  if (newPosInBase < 0) return newPosInBase;
  Position = newPosInBase - BaseStreamOffset;
  return Position;
}

IoError UncompressedStream::Duplicate(InputStream** outStream) {
  InputStream* baseDup;
  IoError err = BaseStream->Duplicate(&baseDup);
  if (err != IoError_OK) return err;
  UncompressedStream* result = new UncompressedStream(*this);
  result->BaseStream = baseDup;
  *outStream = (InputStream*)result;
  return IoError_OK;
}

}  // namespace Io
}  // namespace Impacto