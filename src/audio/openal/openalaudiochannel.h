#pragma once

#include "audiocommon.h"
#include "../audiochannel.h"

namespace Impacto {
namespace Audio {
namespace OpenAL {

class OpenALAudioChannel : public Audio::AudioChannel {
 public:
  OpenALAudioChannel(AudioChannelId id, AudioChannelGroup group);
  ~OpenALAudioChannel();

  void Play(AudioStream* stream, bool loop, float fadeInDuration) override;
  void Stop(float fadeOutDuration) override;
  void Pause() override;
  void Resume() override;

  void Update(float dt) override;

  float PositionInSeconds() const override;

 private:
  static int constexpr AudioBufferSize = 64 * 1024;
  static int constexpr AudioBufferCount = 3;

  ALCuint Source = 0;
  std::array<ALuint, AudioBufferCount> AudioBuffers;
  std::array<size_t, AudioBufferCount> BufferStartPositions;
  std::vector<uint8_t> HostBuffer;

  size_t FirstQueuedBufferIndex = 0;
  bool PlaybackStarted = false;

  float FadeDuration = 0;
  float FadeProgress = 0;

  void EndPlayback();
  void UpdateGain();
  void UpdateFade(float dt);
  void UnqueueBuffers(size_t amount);
  bool CanQueueBuffer();
  void QueueBuffer(size_t bufferIndex);
};

}  // namespace OpenAL
}  // namespace Audio
}  // namespace Impacto