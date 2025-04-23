#pragma once

#include "audiocommon.h"
#include "audiostream.h"
#include <string>
#include <cstdint>

namespace Impacto {
namespace Audio {

class AudioChannel {
 public:
  static std::unique_ptr<AudioChannel> Create(AudioChannelId id,
                                              AudioChannelGroup group);

  virtual void Play(std::unique_ptr<AudioStream> stream, bool loop,
                    float fadeInDuration) = 0;
  void Play(std::string const& mountpoint, std::string const& fileName,
            bool loop, float fadeInDuration);
  void Play(std::string const& mountpoint, uint32_t fileId, bool loop,
            float fadeInDuration);

  virtual void Stop(float fadeOutDuration) = 0;
  virtual void Pause() = 0;
  virtual void Resume() = 0;

  virtual void Update(float dt) = 0;
  virtual float PositionInSeconds() const = 0;

  void SetLooping(bool looping) { Looping = looping; }
  bool IsLooping() { return Looping; }

  AudioChannelState GetState() const { return State; }
  AudioChannelGroup GetGroup() const { return Group; }
  AudioChannelId GetId() const { return Id; }

  float GetVolume() const { return Volume; }
  void SetVolume(float volume) { Volume = volume; }

  const AudioStream* GetStream() const { return Stream.get(); }

  // may be negative for no fixed duration, 0 for no audio
  float DurationInSeconds() const;

 protected:
  AudioChannel(AudioChannelId id, AudioChannelGroup group)
      : Id(id), Group(group) {}

  AudioChannelId Id;
  AudioChannelGroup Group;
  AudioChannelState State = ACS_Stopped;

  float Volume = 1.0f;
  bool Looping = false;

  std::unique_ptr<AudioStream> Stream;
};

class EmptyAudioChannel : public AudioChannel {
 public:
  EmptyAudioChannel(AudioChannelId id, AudioChannelGroup group)
      : AudioChannel(id, group) {}

  void Play(std::unique_ptr<AudioStream> stream, bool loop,
            float fadeInDuration) override {
    State = ACS_Playing;
  }

  void Stop(float fadeOutDuration) override {
    if (State == ACS_Playing || State == ACS_FadingIn || State == ACS_FadingOut)
      State = ACS_Stopped;
  }

  void Pause() override {
    if (State == ACS_Playing || State == ACS_FadingIn || State == ACS_FadingOut)
      State = ACS_Paused;
  }

  void Resume() override {
    if (State == ACS_Paused) State = ACS_Playing;
  }

  void Update(float dt) override {}

  float PositionInSeconds() const override { return 0; }
};

}  // namespace Audio
}  // namespace Impacto
