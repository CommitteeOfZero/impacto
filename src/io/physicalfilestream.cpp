#include "physicalfilestream.h"

#include <algorithm>

namespace Impacto {
namespace Io {

PhysicalFileStream::~PhysicalFileStream() { SDL_RWclose(RW); }

IoError PhysicalFileStream::Create(std::string const& fileName,
                                   InputStream** out) {
  SDL_RWops* rw = SDL_RWFromFile(fileName.c_str(), "rb");
  if (!rw) return IoError_Fail;
  int64_t size = SDL_RWsize(rw);
  if (size <= 0) return IoError_Fail;
  PhysicalFileStream* result = new PhysicalFileStream;
  result->RW = rw;
  result->Meta.Size = size;
  result->SourceFileName = fileName;
  result->Meta.FileName = fileName;
  *out = (InputStream*)result;
  return IoError_OK;
}

int64_t PhysicalFileStream::Read(void* buffer, int64_t sz) {
  if (sz < 0) return IoError_Fail;
  if (Position == Meta.Size) return IoError_Eof;
  sz = std::min(Meta.Size - Position, sz);
  sz = SDL_RWread(RW, buffer, 1, sz);
  Position += sz;
  return sz;
}

int64_t PhysicalFileStream::Seek(int64_t offset, int origin) {
  int64_t newPos = SDL_RWseek(RW, offset, origin);
  if (newPos < 0) return IoError_Fail;
  Position = newPos;
  return newPos;
}

IoError PhysicalFileStream::Duplicate(InputStream** outStream) {
  SDL_RWops* rw = SDL_RWFromFile(SourceFileName.c_str(), "rb");
  if (!rw) return IoError_Fail;
  int64_t pos = SDL_RWseek(rw, Position, RW_SEEK_SET);
  if (pos != Position) {
    SDL_RWclose(rw);
    return IoError_Fail;
  }
  PhysicalFileStream* result = new PhysicalFileStream(*this);
  result->RW = rw;
  *outStream = (InputStream*)result;
  return IoError_OK;
}

}  // namespace Io
}  // namespace Impacto