#include "audiochannel.h"
#include "../audiosystem.h"
#include "../audiostream.h"
#include "../../io/io.h"
#include "../../log.h"

#include <kos.h>
#include <oggvorbis/sndoggvorbis.h>

namespace Impacto {
namespace Audio {
namespace Dreamcast {

AudioChannel::~AudioChannel() {
  if (!IsInit) return;
}

void AudioChannel::Init(AudioChannelId id, AudioChannelGroup group) {
  assert(IsInit == false);
  Id = id;
  Group = group;
  State = ACS_Stopped;

  IsInit = true;
}

void AudioChannel::Play(AudioStream* stream, bool loop, float fadeInDuration) {
  if (!IsInit) return;
  Stop(0.0f);

  Position = 0;

  // Hacks ahead...
  Io::FileMeta* meta = &stream->BaseStream->Meta;
  if (meta->ArchiveFileName.find(".cls") != std::string::npos) {
    std::string basePath =
        meta->ArchiveFileName.substr(
            0, meta->ArchiveFileName.length() - std::string(".cls").length()) +
        "/";
    std::string fullPath = "/cd/" + basePath + meta->FileName;
    sndoggvorbis_start(fullPath.c_str(), loop);
  }
  delete stream;
}

// TODO what to do when already fading out?
void AudioChannel::Stop(float fadeOutDuration) {
  if (!IsInit) return;
  sndoggvorbis_stop();
}

void AudioChannel::Update(float dt) {
  if (!IsInit) return;
}

void AudioChannel::FillBuffers() {}

float AudioChannel::PositionInSeconds() const { return 0.0f; }
float AudioChannel::DurationInSeconds() const { return 0.0f; }

}  // namespace Dreamcast
}  // namespace Audio
}  // namespace Impacto