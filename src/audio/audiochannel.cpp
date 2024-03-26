#include "audiochannel.h"

#include "audiostream.h"

#include "../io/inputstream.h"
#include "../io/vfs.h"
#include "../log.h"

namespace Impacto {
namespace Audio {

void AudioChannel::Play(std::string const& mountpoint,
                        std::string const& fileName, bool loop,
                        float fadeInDuration) {
  Io::InputStream* stream;
  IoError err = Io::VfsOpen(mountpoint, fileName, &stream);
  if (err == IoError_OK) {
    Play(AudioStream::Create(stream), loop, fadeInDuration);
  } else {
    ImpLog(LL_Error, LC_Audio,
           "Could not open audio file %s from mountpoint %s!\n",
           fileName.c_str(), mountpoint.c_str());
  }
}

void AudioChannel::Play(std::string const& mountpoint, uint32_t fileId,
                        bool loop, float fadeInDuration) {
  Io::InputStream* stream;
  IoError err = Io::VfsOpen(mountpoint, fileId, &stream);
  if (err == IoError_OK) {
    Play(AudioStream::Create(stream), loop, fadeInDuration);
  } else {
    ImpLog(LL_Error, LC_Audio,
           "Could not open audio file with ID %d from mountpoint %s!\n", fileId,
           mountpoint.c_str());
  }
}

}  // namespace Audio
}  // namespace Impacto