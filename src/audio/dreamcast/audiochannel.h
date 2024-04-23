#pragma once

#include "../audiochannel.h"

namespace Impacto {
namespace Audio {
namespace Dreamcast {

class AudioChannel : public Audio::AudioChannel {
 public:
  ~AudioChannel();

  void Init(AudioChannelId id, AudioChannelGroup group) override;

  // Stream is automatically deleted when playback is stopped
  void Play(AudioStream* stream, bool loop, float fadeInDuration) override;
  void FillBuffers() override;
  void Stop(float fadeOutDuration) override;

  void Update(float dt) override;

  float PositionInSeconds() const override;
  // may be negative for no fixed duration, 0 for no audio
  float DurationInSeconds() const override;

 private:
  bool IsInit = false;
};

}  // namespace Dreamcast
}  // namespace Audio
}  // namespace Impacto