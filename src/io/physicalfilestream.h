#pragma once

#include "inputstream.h"
#include <SDL_rwops.h>
#include "buffering.h"

namespace Impacto {
namespace Io {

// TODO *optional* buffering

class PhysicalFileStream : public InputStream,
                           public Buffering<PhysicalFileStream> {
  friend class Buffering<PhysicalFileStream>;

 public:
  ~PhysicalFileStream();

  static IoError Create(std::string const& fileName, InputStream** out);
  int64_t Read(void* buffer, int64_t sz) override;
  int64_t Seek(int64_t offset, int origin) override;
  IoError Duplicate(InputStream** outStream) override;

 protected:
  static int const PhysicalBufferSize = 16 * 1024;

  PhysicalFileStream() : Buffering(PhysicalBufferSize) {}
  PhysicalFileStream(PhysicalFileStream const& other) = default;

  IoError FillBuffer();

  SDL_RWops* RW;
  std::string SourceFileName;
};

}  // namespace Io
}  // namespace Impacto