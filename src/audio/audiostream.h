#pragma once

#include "audiocommon.h"
#include "../impacto.h"

namespace Impacto {
namespace Audio {

class AudioStream {
 public:
  virtual ~AudioStream() {
    if (BaseStream) SDL_RWclose(BaseStream);
  }

  static AudioStream* Create(SDL_RWops* stream);

  // Returns number of samples read
  virtual int Read(void* buffer, int samples) = 0;
  virtual void Seek(int samples) = 0;

  int BytesPerSample() const { return (BitDepth / 8) * ChannelCount; }

  int ChannelCount;
  int SampleRate;
  int BitDepth;

  int LoopStart = 0;
  int LoopEnd = 0;
  // negative indicates no fixed length
  int Duration = -1;
  int ReadPosition = 0;

 protected:
  AudioStream(SDL_RWops* stream) : BaseStream(stream) {}

  SDL_RWops* const BaseStream;
};

}  // namespace Audio
}  // namespace Impacto