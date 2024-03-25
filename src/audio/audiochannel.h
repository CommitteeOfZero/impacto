#pragma once

#include "audiocommon.h"

namespace Impacto {
namespace Audio {

class AudioChannel {
 public:
  virtual ~AudioChannel(){};

  virtual void Init(AudioChannelId id, AudioChannelGroup group) {
    Id = id;
    Group = group;
    State = ACS_Stopped;
  };

  // Stream is automatically deleted when playback is stopped
  virtual void Play(AudioStream* stream, bool loop, float fadeInDuration){};
  virtual void FillBuffers(){};
  virtual void Stop(float fadeOutDuration){};

  virtual void Update(float dt){};

  virtual float PositionInSeconds() const { return 0.0f; };
  // may be negative for no fixed duration, 0 for no audio
  virtual float DurationInSeconds() const { return 0.0f; };

  AudioChannelId Id;
  AudioChannelGroup Group;

  AudioChannelState State;

  float Volume = 1.0f;

  // Read only - seeking is currently not supported
  // Actual playhead at start of (graphics) frame, in AudioStream samples
  int Position = 0;

  bool Looping;
};

}  // namespace Audio
}  // namespace Impacto