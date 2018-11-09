#pragma once

#include "audiocommon.h"

namespace Impacto {
namespace Audio {

class AudioChannel {
 public:
  ~AudioChannel();

  void Init(AudioChannelId id, AudioChannelGroup group);

  // Stream is automatically deleted when playback is stopped
  void Play(AudioStream* stream, bool loop, float fadeInDuration);
  void FillBuffers();
  void Stop(float fadeOutDuration);

  void Update(float dt);

  float PositionInSeconds() const;

  AudioChannelId Id;
  AudioChannelGroup Group;

  AudioChannelState State;

  float Volume = 1.0f;

  // Read only - seeking is currently not supported
  // Actual playhead at start of (graphics) frame, in AudioStream samples
  int Position = 0;

 private:
  void SetGain();
  int SamplesPerBuffer() const;

  static int const AudioBufferSize = 64 * 1024;
  static int const AudioBufferCount = 3;

  ALuint BufferIds[AudioBufferCount];
  uint8_t HostBuffer[AudioBufferSize];
  int FirstFreeBuffer;
  // Unqueued buffers that have not been filled since being dequeued
  int FreeBufferCount;
  // In AudioStream samples
  int BufferStartPositions[AudioBufferCount];

  AudioStream* CurrentStream = 0;
  bool IsInit = false;

  bool Looping;
  bool FinishedDecode;

  float FadeDuration = 0.0f;
  float FadeCompletion = 0.0f;

  ALCuint Source;
};

}  // namespace Audio
}  // namespace Impacto