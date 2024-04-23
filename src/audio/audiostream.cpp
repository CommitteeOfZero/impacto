#include "audiostream.h"
#include "../log.h"

namespace Impacto {
namespace Audio {

#ifdef PLATFORM_DREAMCAST
int CurrentRegistryIdx = 0;
#endif

AudioStream* AudioStream::Create(Io::InputStream* stream) {
#ifdef PLATFORM_DREAMCAST
  for (int i = 0; i < CurrentRegistryIdx; i++) {
    AudioStream* result = Registry[i](stream);
    if (result) return result;
  }
#else
  for (auto f : Registry) {
    AudioStream* result = f(stream);
    if (result) return result;
  }
#endif
  ImpLog(LL_Error, LC_Audio, "No audio decoder found\n");
  return 0;
}

bool AudioStream::AddAudioStreamCreator(AudioStreamCreator c) {
#ifdef PLATFORM_DREAMCAST
  if (CurrentRegistryIdx == AuidoStreamRegistrySize) return true;

  Registry[CurrentRegistryIdx++] = c;
#else
  Registry.push_back(c);
#endif
  return true;
}

#ifdef PLATFORM_DREAMCAST
AudioStream::AudioStreamCreator AudioStream::Registry[AuidoStreamRegistrySize];
#else
std::vector<AudioStream::AudioStreamCreator> AudioStream::Registry;
#endif

}  // namespace Audio
}  // namespace Impacto