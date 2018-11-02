#pragma once

#include "audiostream.h"
#include "../impacto.h"

namespace Impacto {
namespace Audio {

bool AudioIsAtrac9(SDL_RWops* stream);

class Atrac9AudioStream : public AudioStream {
 public:
  Atrac9AudioStream(SDL_RWops* stream);
  ~Atrac9AudioStream();

  int Read(void* buffer, int samples) override;
  void Seek(int samples) override;

 private:
  void* At9 = 0;

  int16_t* DecodedBuffer = 0;
  int DecodedSamplesAvailable = 0;
  int DecodedSamplesConsumed = 0;
  uint8_t* EncodedBuffer = 0;
  int EncodedBufferSize = 0;

  int StreamDataOffset;
  int SamplesPerFrame;
  int FramesPerSuperframe;
  int EncoderDelay;
};

}  // namespace Audio
}  // namespace Impacto