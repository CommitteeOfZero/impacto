#pragma once

#include "stream.h"
#include <SDL_rwops.h>
#include "buffering.h"

namespace Impacto {
namespace Io {

// TODO *optional* buffering

class PhysicalFileStream : public Stream, public Buffering<PhysicalFileStream> {
  friend class Buffering<PhysicalFileStream>;

 public:
  ~PhysicalFileStream();

  static IoError Create(std::string const& fileName, Stream** out);
  static IoError CreateWrite(std::string const& fileName, Stream** out);
  int64_t Read(void* buffer, int64_t sz) override;
  int64_t Seek(int64_t offset, int origin) override;
  IoError Duplicate(Stream** outStream) override;
  int64_t Write(void* buffer, int64_t sz, int cnt = 1) override;

 protected:
  static int constexpr PhysicalBufferSize = 16 * 1024;
  bool IsWrite = false;
  PhysicalFileStream() : Buffering(PhysicalBufferSize) {}
  PhysicalFileStream(PhysicalFileStream const& other) = default;

  IoError FillBuffer();
  IoError FlushBuffer();

  SDL_RWops* RW;
  std::string SourceFileName;
};

}  // namespace Io
}  // namespace Impacto