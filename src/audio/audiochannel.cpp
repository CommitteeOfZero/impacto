#include "audiochannel.h"
#include "audiostream.h"

#ifndef IMPACTO_DISABLE_OPENAL
#include "openal/openalaudiochannel.h"
#endif

#include "../io/stream.h"
#include "../io/vfs.h"
#include "../log.h"
#include "../profile/game.h"

namespace Impacto {
namespace Audio {

std::unique_ptr<AudioChannel> AudioChannel::Create(
    AudioChannelId channelId, AudioChannelGroup channelGroup) {
  switch (Profile::ActiveAudioBackend) {
#ifndef IMPACTO_DISABLE_OPENAL
    case AudioBackendType::OpenAL: {
      return std::make_unique<OpenAL::OpenALAudioChannel>(channelId,
                                                          channelGroup);
    } break;
#endif
    case AudioBackendType::None:
    default:
      return std::make_unique<EmptyAudioChannel>(channelId, channelGroup);
  }
}

void AudioChannel::Play(std::string const& mountpoint,
                        std::string const& fileName, bool loop,
                        float fadeInDuration) {
  Io::Stream* stream;
  IoError err = Io::VfsOpen(mountpoint, fileName, &stream);
  if (err == IoError_OK) {
    Play(std::unique_ptr<AudioStream>(AudioStream::Create(stream)), loop,
         fadeInDuration);
  } else {
    ImpLog(LogLevel::Error, LogChannel::Audio,
           "Could not open audio file {:s} from mountpoint {:s}!\n", fileName,
           mountpoint);
  }
}

void AudioChannel::Play(std::string const& mountpoint, uint32_t fileId,
                        bool loop, float fadeInDuration) {
  Io::Stream* stream;
  IoError err = Io::VfsOpen(mountpoint, fileId, &stream);
  if (err == IoError_OK) {
    Play(std::unique_ptr<AudioStream>(AudioStream::Create(stream)), loop,
         fadeInDuration);
  } else {
    ImpLog(LogLevel::Error, LogChannel::Audio,
           "Could not open audio file with ID {:d} from mountpoint {:s}!\n",
           fileId, mountpoint);
  }
}

float AudioChannel::DurationInSeconds() const {
  if (!Stream) return 0;
  return (float)Stream->Duration / (float)Stream->SampleRate;
}

}  // namespace Audio
}  // namespace Impacto