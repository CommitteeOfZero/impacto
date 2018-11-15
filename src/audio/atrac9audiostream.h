#pragma once

#include "audiostream.h"
#include "../impacto.h"
#include "buffering.h"
#include <libatrac9/libatrac9.h>

namespace Impacto {
namespace Audio {

struct At9ContainerInfo;

class Atrac9AudioStream : public AudioStream,
                          public Buffering<Atrac9AudioStream, int16_t> {
  friend class Buffering<Atrac9AudioStream, int16_t>;

 public:
  ~Atrac9AudioStream();

  int Read(void* buffer, int samples) override;
  void Seek(int samples) override;

 protected:
  bool DecodeBuffer();

 private:
  static AudioStream* Create(Io::InputStream* stream);
  Atrac9AudioStream() {}

  void InitWithInfo(At9ContainerInfo* container, Atrac9CodecInfo* codecinfo);

  void* At9 = 0;
  int SamplesPerFrame;
  int FramesPerSuperframe;
  int EncoderDelay;

  static bool _registered;
};

}  // namespace Audio
}  // namespace Impacto