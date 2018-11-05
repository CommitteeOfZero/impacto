#pragma once

#include "audiostream.h"
#include "../impacto.h"
#include "buffering.h"

namespace Impacto {
namespace Audio {

bool AudioIsAtrac9(SDL_RWops* stream);

class Atrac9AudioStream : public AudioStream,
                          public Buffering<Atrac9AudioStream, int16_t> {
  friend class Buffering<Atrac9AudioStream, int16_t>;

 public:
  Atrac9AudioStream(SDL_RWops* stream);
  ~Atrac9AudioStream();

  int Read(void* buffer, int samples) override;
  void Seek(int samples) override;

 protected:
  bool DecodeBuffer();
  int16_t* DecodedBuffer = 0;
  uint8_t* EncodedBuffer = 0;

 private:
  void* At9 = 0;
  int SamplesPerFrame;
  int FramesPerSuperframe;
  int EncoderDelay;
};

}  // namespace Audio
}  // namespace Impacto