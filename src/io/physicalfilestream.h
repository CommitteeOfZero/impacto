#pragma once

#include "inputstream.h"
#include <SDL_rwops.h>

namespace Impacto {
namespace Io {

class PhysicalFileStream : public InputStream {
 public:
  ~PhysicalFileStream();

  static IoError Create(std::string const& fileName, InputStream** out);
  int64_t Read(void* buffer, int64_t sz) override;
  int64_t Seek(int64_t offset, int origin) override;
  IoError Duplicate(InputStream** outStream) override;

 protected:
  PhysicalFileStream() {}
  PhysicalFileStream(PhysicalFileStream const& other) = default;

  SDL_RWops* RW;
  std::string SourceFileName;
};

}  // namespace Io
}  // namespace Impacto