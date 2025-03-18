#include "audiostream.h"
#include "../log.h"

namespace Impacto {
namespace Audio {

std::vector<AudioStream::AudioStreamCreator>& AudioStream::GetRegistry() {
  static std::vector<AudioStreamCreator> registry;
  return registry;
}

AudioStream* AudioStream::Create(Io::Stream* stream) {
  for (auto f : GetRegistry()) {
    AudioStream* result = f(stream);
    if (result) return result;
  }
  ImpLog(LogLevel::Error, LogChannel::Audio, "No audio decoder found\n");
  return 0;
}

bool AudioStream::AddAudioStreamCreator(AudioStreamCreator c) {
  GetRegistry().push_back(c);
  return true;
}

}  // namespace Audio
}  // namespace Impacto