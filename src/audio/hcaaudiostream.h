#pragma once

#include "audiostream.h"
#include "../impacto.h"
#include "buffering.h"

#include <clHCA/clHCA.h>

namespace Impacto {
namespace Audio {

class HcaAudioStream : public AudioStream,
                       public Buffering<HcaAudioStream, int16_t> {
  friend class Buffering<HcaAudioStream, int16_t>;

 public:
  ~HcaAudioStream();

  int Read(void* buffer, int samples) override;
  void Seek(int samples) override;

 protected:
  bool DecodeBuffer();

 private:
  static AudioStream* Create(Io::Stream* stream);
  HcaAudioStream() {}

  void InitWithInfo(clHCA_stInfo* info);

  clHCA* Decoder = 0;

  static bool _registered;
};

}  // namespace Audio
}  // namespace Impacto