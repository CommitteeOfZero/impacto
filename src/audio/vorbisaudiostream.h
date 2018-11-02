#pragma once

#include "audiostream.h"
#include "../impacto.h"
#include <vorbis/vorbisfile.h>

namespace Impacto {
namespace Audio {

bool AudioIsVorbis(SDL_RWops* stream);

class VorbisAudioStream : public AudioStream {
 public:
  VorbisAudioStream(SDL_RWops* stream);
  ~VorbisAudioStream();

  int Read(void* buffer, int samples) override;
  void Seek(int samples) override;

  bool VfOpen = false;

 private:
  OggVorbis_File Vf;
};

}  // namespace Audio
}  // namespace Impacto