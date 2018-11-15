#include "audiostream.h"
#include "../log.h"

namespace Impacto {
namespace Audio {

AudioStream* AudioStream::Create(Io::InputStream* stream) {
  for (auto f : Registry) {
    AudioStream* result = f(stream);
    if (result) return result;
  }
  ImpLog(LL_Error, LC_Audio, "No audio decoder found\n");
  return 0;
}

bool AudioStream::AddAudioStreamCreator(AudioStreamCreator c) {
  Registry.push_back(c);
  return true;
}

std::vector<AudioStream::AudioStreamCreator> AudioStream::Registry;

}  // namespace Audio
}  // namespace Impacto