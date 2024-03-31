#include <algorithm>

#include "audiochannel.h"
#include "../audiosystem.h"
#include "../audiostream.h"
#include "../../log.h"

namespace Impacto {
namespace Audio {
namespace OpenAL {

static ALenum ToALFormat(int channels, int bitdepth) {
  if (channels == 2 && bitdepth == 16) return AL_FORMAT_STEREO16;
  if (channels == 2 && bitdepth == 32) return AL_FORMAT_STEREO_FLOAT32;
  if (channels == 1 && bitdepth == 16) return AL_FORMAT_MONO16;
  if (channels == 1 && bitdepth == 32) return AL_FORMAT_MONO_FLOAT32;
  if (channels == 2 && bitdepth == 8) return AL_FORMAT_STEREO8;
  if (channels == 1 && bitdepth == 8) return AL_FORMAT_MONO8;

  assert(false);
}

AudioChannel::~AudioChannel() {
  if (!IsInit) return;
  Stop(0.0f);
  alDeleteSources(1, &Source);
  alDeleteBuffers(AudioBufferCount, BufferIds);
}

void AudioChannel::Init(AudioChannelId id, AudioChannelGroup group) {
  assert(IsInit == false);
  Id = id;
  Group = group;
  State = ACS_Stopped;
  CurrentStream = 0;

  alGenSources(1, &Source);
  alSourcef(Source, AL_PITCH, 1);
  alSourcef(Source, AL_GAIN, 0);
  alSource3f(Source, AL_POSITION, 0, 0, 0);
  alSource3f(Source, AL_VELOCITY, 0, 0, 0);
  alSourcei(Source, AL_LOOPING, AL_FALSE);

  alGenBuffers(AudioBufferCount, BufferIds);

  IsInit = true;
}

// TODO AudioStream creation on background thread
void AudioChannel::Play(AudioStream* stream, bool loop, float fadeInDuration) {
  if (!IsInit) return;
  assert(fadeInDuration >= 0.0f);
  Stop(0.0f);

  CurrentStream = stream;
  Looping = loop;
  FreeBufferCount = AudioBufferCount;
  FirstFreeBuffer = 0;
  memset(BufferStartPositions, 0, sizeof(BufferStartPositions));
  FinishedDecode = false;
  Position = 0;

  State = ACS_FadingIn;
  FadeDuration = fadeInDuration;
  if (fadeInDuration > 0.0f) {
    FadeCompletion = 0.0f;
  } else {
    // Still FadingIn so FillBuffers doesn't think we're underrunning
    FadeCompletion = 1.0f;
  }
  SetGain();

  FillBuffers();
  alSourcePlay(Source);
}

// TODO what to do when already fading out?
void AudioChannel::Stop(float fadeOutDuration) {
  if (!IsInit) return;
  assert(fadeOutDuration >= 0.0f);
  if (State == ACS_Stopped) return;
  if (fadeOutDuration == 0.0f) {
    State = ACS_Stopped;
    // unqueue all buffers
    alSourcei(Source, AL_BUFFER, NULL);
    alSourceStop(Source);
    // ugh, leftover state
    alDeleteSources(1, &Source);
    alGenSources(1, &Source);
    if (CurrentStream) {
      delete CurrentStream;
      CurrentStream = 0;
    }
    Position = 0;
  } else {
    State = ACS_FadingOut;
    FadeDuration = fadeOutDuration;
    FadeCompletion = 0.0f;
  }
}

void AudioChannel::Update(float dt) {
  if (!IsInit) return;

  // Update fade

  if (State == ACS_FadingIn || State == ACS_FadingOut) {
    FadeCompletion += dt / FadeDuration;
    if (FadeCompletion >= 1.0f) {
      if (State == ACS_FadingIn) {
        State = ACS_Playing;
      } else {
        Stop(0);
      }
    }
  }
  if (State == ACS_Stopped) return;
  SetGain();

  // Update playhead and stop playing if we're done

  alGetSourcei(Source, AL_BUFFERS_PROCESSED, &FreeBufferCount);

  if (FreeBufferCount == AudioBufferCount && FinishedDecode && !Looping) {
    // whole file has been played out
    Stop(0.0f);
    return;
  }

  // The first buffer we can't write to = currently playing buffer
  int currentlyPlayingBuffer =
      (FirstFreeBuffer + FreeBufferCount) % AudioBufferCount;

  // TODO: Figure out what's wrong with this, seems like position can randomly
  // become less than the previous position (I think start + offset somehow gets
  // bigger than next buffer start)
  int offset;
  alGetSourcei(Source, AL_SAMPLE_OFFSET, &offset);
  Position = BufferStartPositions[currentlyPlayingBuffer] + offset;
  if (Looping) {
    if (Position > CurrentStream->LoopEnd)
      Position =
          CurrentStream->LoopStart +
          (Position % (CurrentStream->LoopEnd - CurrentStream->LoopStart));
  } else {
    Position = std::min(Position, CurrentStream->Duration);
  }

  // TODO do this on background thread detached from game loop
  FillBuffers();

  // restart playback after underrun

  ALint sourceState;
  alGetSourcei(Source, AL_SOURCE_STATE, &sourceState);
  if (State != ACS_Stopped && sourceState != AL_PLAYING) {
    ImpLog(LL_Error, LC_Audio,
           "Restarting playback after buffer underrun on channel %d - %d\n", Id,
           sourceState);
    alSourcePlay(Source);
  }
}

// TODO what easing functions do we want for this?
void AudioChannel::SetGain() {
  float gain = MasterVolume * GroupVolumes[Group] * Volume;
  switch (State) {
    case ACS_Stopped:
    case ACS_Playing:
      return;
    case ACS_FadingIn:
      gain *= powf(FadeCompletion, 3.0f);
      break;
    case ACS_FadingOut:
      gain *= 1.0f - powf(FadeCompletion, 3.0f);
      break;
  }
  alSourcef(Source, AL_GAIN, gain);
}

// TODO restart playback on underrun, e.g.
// https://github.com/arx/ArxLibertatis/blob/master/src/audio/openal/OpenALSource.cpp
void AudioChannel::FillBuffers() {
  if (!CurrentStream) return;

  if (FreeBufferCount == AudioBufferCount && !FinishedDecode &&
      State == ACS_Playing) {
    ImpLog(LL_Error, LC_Audio, "Buffer underrun on channel %d\n", Id);
  }

  int maxSamples = SamplesPerBuffer();

  while (FreeBufferCount) {
    alSourceUnqueueBuffers(Source, 1, &BufferIds[FirstFreeBuffer]);
    FreeBufferCount--;

    BufferStartPositions[FirstFreeBuffer] = CurrentStream->ReadPosition;

    memset(HostBuffer, 0, AudioBufferSize);

    uint8_t* dest;
    int samplesRead = 0;
    while (samplesRead < maxSamples) {
      if (Looping && CurrentStream->ReadPosition >= CurrentStream->LoopEnd) {
        ImpLog(LL_Trace, LC_Audio, "Channel %d looping\n", Id);
        CurrentStream->Seek(CurrentStream->LoopStart);
      }
      dest = HostBuffer + samplesRead * CurrentStream->BytesPerSample();
      int samplesToRead = maxSamples - samplesRead;
      if (Looping)
        samplesToRead =
            std::min(samplesToRead,
                     CurrentStream->LoopEnd - CurrentStream->ReadPosition);
      int samplesReadThisIteration = CurrentStream->Read(dest, samplesToRead);
      samplesRead += samplesReadThisIteration;

      if (CurrentStream->ReadPosition >= CurrentStream->Duration &&
          CurrentStream->Duration >= 0) {
        FinishedDecode = true;
      }
      if (samplesReadThisIteration == 0 && FinishedDecode) break;
    }

    alBufferData(
        BufferIds[FirstFreeBuffer],
        ToALFormat(CurrentStream->ChannelCount, CurrentStream->BitDepth),
        HostBuffer, SamplesPerBuffer() * CurrentStream->BytesPerSample(),
        CurrentStream->SampleRate);
    alSourceQueueBuffers(Source, 1, &BufferIds[FirstFreeBuffer]);

    FirstFreeBuffer++;
    FirstFreeBuffer %= AudioBufferCount;
  }
}

int AudioChannel::SamplesPerBuffer() const {
  return AudioBufferSize / CurrentStream->BytesPerSample();
}

float AudioChannel::PositionInSeconds() const {
  if (!CurrentStream) return 0;
  return (float)Position / (float)CurrentStream->SampleRate;
}
float AudioChannel::DurationInSeconds() const {
  if (!CurrentStream) return 0;
  return (float)CurrentStream->Duration / (float)CurrentStream->SampleRate;
}

}  // namespace OpenAL
}  // namespace Audio
}  // namespace Impacto