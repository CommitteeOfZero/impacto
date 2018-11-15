#pragma once

#include "audiostream.h"
#include "../impacto.h"
#include <vorbis/vorbisfile.h>

namespace Impacto {
namespace Audio {

class VorbisAudioStream : public AudioStream {
 public:
  ~VorbisAudioStream();

  int Read(void* buffer, int samples) override;
  void Seek(int samples) override;

  bool VfOpen = false;

 private:
  static AudioStream* Create(Io::InputStream* stream);
  VorbisAudioStream() {}
  void InitWithInfo(vorbis_info* info);

  OggVorbis_File Vf;

  static bool _registered;
};

}  // namespace Audio
}  // namespace Impacto