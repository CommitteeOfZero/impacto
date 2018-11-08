#pragma once

#include "audiostream.h"
#include "../impacto.h"
#include "buffering.h"

namespace Impacto {
namespace Audio {

struct AdxHeaderInfo;

class AdxAudioStream : public AudioStream,
                       public Buffering<AdxAudioStream, int16_t> {
  friend class Buffering<AdxAudioStream, int16_t>;

 public:
  ~AdxAudioStream();

  int Read(void* buffer, int samples) override;
  void Seek(int samples) override;

 protected:
  bool DecodeBuffer();
  int16_t DecodedBuffer[127] = {0};
  uint8_t EncodedBuffer[256] = {0};

 private:
  static AudioStream* Create(SDL_RWops* stream);
  AdxAudioStream() {}
  void InitWithInfo(AdxHeaderInfo* info);
  void SetCoefficients(double cutoff, double sampleRate);

  int32_t Coef1;
  int32_t Coef2;

  int32_t Hist1[2];
  int32_t Hist2[2];

  static bool _registered;
};

}  // namespace Audio
}  // namespace Impacto