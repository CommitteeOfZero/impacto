#pragma once

#include "audiostream.h"
#include "../impacto.h"
#include "buffering.h"

namespace Impacto {
namespace Audio {

bool AudioIsAdx(SDL_RWops* stream);

class AdxAudioStream : public AudioStream,
                       public Buffering<AdxAudioStream, int16_t> {
  friend class Buffering<AdxAudioStream, int16_t>;

 public:
  AdxAudioStream(SDL_RWops* stream);
  ~AdxAudioStream();

  int Read(void* buffer, int samples) override;
  void Seek(int samples) override;

 protected:
  bool DecodeBuffer();
  int16_t DecodedBuffer[127] = {0};
  uint8_t EncodedBuffer[256] = {0};

 private:
  void SetCoefficients(double cutoff, double sampleRate);
  bool DecodeBufferMono();
  bool DecodeBufferStereo();

  int32_t Coef1;
  int32_t Coef2;

  int32_t Hist1_L;
  int32_t Hist2_L;
  int32_t Hist1_R;
  int32_t Hist2_R;
};

}  // namespace Audio
}  // namespace Impacto