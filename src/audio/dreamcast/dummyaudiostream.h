#pragma once

#include "../audiostream.h"
#include "../../impacto.h"

namespace Impacto {
namespace Audio {

class DummyAudioStream : public AudioStream {
 public:
  ~DummyAudioStream();

  int Read(void* buffer, int samples) override;
  void Seek(int samples) override;

  bool VfOpen = false;

 private:
  static AudioStream* Create(Io::InputStream* stream);
  DummyAudioStream() {}

  static bool _registered;
};

}  // namespace Audio
}  // namespace Impacto