#pragma once

#include "audiocommon.h"
#include "../audiochannel.h"

namespace Impacto {
namespace Audio {
namespace OpenAL {

class AudioChannel : public Audio::AudioChannel {
 public:
  ~AudioChannel();

  void Init(AudioChannelId id, AudioChannelGroup group) override;

  // Stream is automatically deleted when playback is stopped
  void Play(AudioStream* stream, bool loop, float fadeInDuration) override;
  void FillBuffers() override;
  void Stop(float fadeOutDuration) override;
  void Pause() override;
  void Resume() override;

  void Update(float dt) override;

  float PositionInSeconds() const override;
  // may be negative for no fixed duration, 0 for no audio
  float DurationInSeconds() const override;

  const AudioStream* GetStream() const override;

 private:
  void SetGain();
  int SamplesPerBuffer() const;

  static int constexpr AudioBufferSize = 64 * 1024;
  static int constexpr AudioBufferCount = 3;

  ALuint BufferIds[AudioBufferCount];
  uint8_t HostBuffer[AudioBufferSize];
  int FirstFreeBuffer;
  // Unqueued buffers that have not been filled since being dequeued
  int FreeBufferCount;
  // In AudioStream samples
  int BufferStartPositions[AudioBufferCount];

  AudioStream* CurrentStream = 0;
  bool IsInit = false;

  bool FinishedDecode;

  float FadeDuration = 0.0f;
  float FadeCompletion = 0.0f;

  ALCuint Source;
};

}  // namespace OpenAL
}  // namespace Audio
}  // namespace Impacto