#pragma once

#include "audiostream.h"
#include "../impacto.h"
#include "buffering.h"

#include "../../vendor/clHCA/clHCA.h"

namespace Impacto {
namespace Audio {

bool AudioIsHca(SDL_RWops* stream);

class HcaAudioStream : public AudioStream,
                       public Buffering<HcaAudioStream, int16_t> {
  friend class Buffering<HcaAudioStream, int16_t>;

 public:
  ~HcaAudioStream();

  int Read(void* buffer, int samples) override;
  void Seek(int samples) override;

 protected:
  bool DecodeBuffer();
  int16_t* DecodedBuffer = 0;
  uint8_t* EncodedBuffer = 0;

 private:
  static AudioStream* Create(SDL_RWops* stream);
  HcaAudioStream() {}

  void InitWithInfo(clHCA_stInfo* info);

  clHCA* Decoder = 0;

  static bool _registered;
};

}  // namespace Audio
}  // namespace Impacto