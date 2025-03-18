#include "audiochannel.h"

#include "audiostream.h"

#include "../io/stream.h"
#include "../io/vfs.h"
#include "../log.h"

namespace Impacto {
namespace Audio {

void AudioChannel::Play(std::string const& mountpoint,
                        std::string const& fileName, bool loop,
                        float fadeInDuration) {
  Io::Stream* stream;
  IoError err = Io::VfsOpen(mountpoint, fileName, &stream);
  if (err == IoError_OK) {
    Play(AudioStream::Create(stream), loop, fadeInDuration);
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
    Play(AudioStream::Create(stream), loop, fadeInDuration);
  } else {
    ImpLog(LogLevel::Error, LogChannel::Audio,
           "Could not open audio file with ID {:d} from mountpoint {:s}!\n",
           fileId, mountpoint);
  }
}

}  // namespace Audio
}  // namespace Impacto