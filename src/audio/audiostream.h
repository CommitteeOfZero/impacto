#pragma once

#include "audiocommon.h"
#include "../impacto.h"

#include <vector>

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
  typedef AudioStream* (*AudioStreamCreator)(SDL_RWops* stream);
  static bool AddAudioStreamCreator(AudioStreamCreator c);

  AudioStream(){};

  SDL_RWops* BaseStream;

 private:
  static std::vector<AudioStreamCreator> Registry;
};

}  // namespace Audio
}  // namespace Impacto