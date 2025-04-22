#include "audiochannel.h"
#include "../audiosystem.h"
#include "../audiostream.h"

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

OpenALAudioChannel::OpenALAudioChannel(AudioChannelId id,
                                       AudioChannelGroup group)
    : AudioChannel(id, group),
      HostBuffer(std::vector(AudioBufferSize, (uint8_t)0)) {
  BufferStartPositions.fill(0);

  alGenBuffers(AudioBuffers.size(), AudioBuffers.data());
  alGenSources(1, &Source);

  alSourcef(Source, AL_PITCH, 1);
  alSource3f(Source, AL_POSITION, 0, 0, 0);
  alSource3f(Source, AL_VELOCITY, 0, 0, 0);
  alSourcei(Source, AL_LOOPING, AL_FALSE);
  UpdateGain();
}

OpenALAudioChannel::~OpenALAudioChannel() {
  if (Stream) delete Stream;

  alDeleteBuffers(AudioBuffers.size(), AudioBuffers.data());
  alDeleteSources(1, &Source);
}

void OpenALAudioChannel::Play(AudioStream* stream, bool loop,
                              float fadeInDuration) {
  EndPlayback();

  Stream = stream;
  FadeDuration = fadeInDuration;
  FadeProgress = FadeDuration == 0 ? 1 : 0;
  State = FadeDuration == 0 ? ACS_Playing : ACS_FadingIn;
  Looping = loop;
}

void OpenALAudioChannel::Stop(float fadeOutDuration) {
  if (fadeOutDuration == 0) {
    EndPlayback();
    return;
  }

  State = ACS_FadingOut;
  FadeDuration = fadeOutDuration;
  FadeProgress = 0;
}

void OpenALAudioChannel::EndPlayback() {
  if (Stream) delete Stream;
  Stream = nullptr;

  alSourceStop(Source);

  ALint queuedBuffers;
  alGetSourcei(Source, AL_BUFFERS_QUEUED, &queuedBuffers);
  UnqueueBuffers(queuedBuffers);

  State = ACS_Stopped;
  FadeProgress = 0;
  FirstQueuedBufferIndex = 0;
  BufferStartPositions.fill(0);
}

void OpenALAudioChannel::Pause() {
  if (State != ACS_Playing && State != ACS_FadingIn && State != ACS_FadingOut) {
    return;
  }

  alSourcePause(Source);
  State = ACS_Paused;
}

void OpenALAudioChannel::Resume() {
  if (State != ACS_Paused) return;

  // Should we fade here?
  State = ACS_Playing;
}

float OpenALAudioChannel::PositionInSeconds() const {
  int offset;
  alGetSourcei(Source, AL_SAMPLE_OFFSET, &offset);

  size_t position = BufferStartPositions[FirstQueuedBufferIndex] + offset;
  return (float)position / Stream->SampleRate;
}

void OpenALAudioChannel::Update(float dt) {
  if (State == ACS_FadingIn || State == ACS_FadingOut) {
    UpdateFade(dt);
  }

  if (State != ACS_Playing && State != ACS_FadingIn && State != ACS_FadingIn) {
    return;
  }

  // Continuously update the gain
  // because the global state might have changed
  UpdateGain();

  // End playback if OpenAL stopped playback
  ALenum alState;
  alGetSourcei(Source, AL_SOURCE_STATE, &alState);
  if (Stream->ReadPosition > 0 && alState == AL_STOPPED) {
    EndPlayback();
    return;
  }

  ALint processedBuffers;
  alGetSourcei(Source, AL_BUFFERS_PROCESSED, &processedBuffers);
  UnqueueBuffers(processedBuffers);

  // TODO do this on background thread
  while (CanQueueBuffer()) {
    int queuedBuffers;
    alGetSourcei(Source, AL_BUFFERS_QUEUED, &queuedBuffers);

    if (queuedBuffers == 0 && Stream->ReadPosition > 0) {
      ImpLog(LogLevel::Error, LogChannel::Audio,
             "Buffer underrun on channel {:d}\n", Id);
    }

    size_t firstFreeIndex =
        (FirstQueuedBufferIndex + queuedBuffers) % AudioBuffers.size();
    QueueBuffer(firstFreeIndex);
  }

  // Start playback if it isn't started yet
  if (alState != AL_PLAYING) {
    alSourcePlay(Source);
  }
}

void OpenALAudioChannel::UpdateFade(float dt) {
  FadeProgress = std::min(1.0f, FadeProgress + dt / FadeDuration);
  if (FadeProgress >= 1.0f) {
    if (State == ACS_FadingIn) {
      State = ACS_Playing;
    } else {
      EndPlayback();
    }
  }
}

// TODO what easing functions do we want for this?
void OpenALAudioChannel::UpdateGain() {
  float gain = MasterVolume * GroupVolumes[Group] * Volume;
  switch (State) {
    case ACS_Stopped:
      return;
    case ACS_Playing:
      // Nothing
      break;
    case ACS_FadingIn:
      gain *= powf(FadeProgress, 3.0f);
      break;
    case ACS_FadingOut:
      gain *= 1.0f - powf(FadeProgress, 3.0f);
      break;
  }

  alSourcef(Source, AL_GAIN, gain);
}

// Unqueues `amount` buffers, starting at the first queued buffer
void OpenALAudioChannel::UnqueueBuffers(size_t amount) {
  for (size_t i = 0; i < amount; i++) {
    size_t bufferIndex = FirstQueuedBufferIndex % AudioBuffers.size();
    alSourceUnqueueBuffers(Source, 1, &AudioBuffers[bufferIndex]);
    FirstQueuedBufferIndex = (bufferIndex + 1) % AudioBuffers.size();
  }
}

bool OpenALAudioChannel::CanQueueBuffer() {
  ALint queuedBuffers;
  alGetSourcei(Source, AL_BUFFERS_QUEUED, &queuedBuffers);

  if (queuedBuffers >= AudioBuffers.size()) return false;

  return Looping || Stream->ReadPosition < Stream->Duration;
}

void OpenALAudioChannel::QueueBuffer(size_t bufferIndex) {
  BufferStartPositions[bufferIndex] = Stream->ReadPosition;
  std::fill(HostBuffer.begin(), HostBuffer.end(), 0);

  int maxSamples = AudioBufferSize / Stream->BytesPerSample();
  int samplesRead = 0;

  while (samplesRead < maxSamples && Stream->ReadPosition < Stream->Duration) {
    int end = Looping ? Stream->LoopEnd : Stream->Duration;
    int samplesToRead = std::min(maxSamples, end - Stream->ReadPosition);

    uint8_t* dest = &HostBuffer[samplesRead * Stream->BytesPerSample()];
    int samplesReadThisIteration = Stream->Read(dest, samplesToRead);
    samplesRead += samplesReadThisIteration;

    if (Looping && Stream->ReadPosition >= Stream->LoopEnd) {
      Stream->Seek(Stream->LoopStart);
    }

    if (samplesReadThisIteration == 0) break;
  }

  ALuint buffer = AudioBuffers[bufferIndex];
  ALenum format = ToALFormat(Stream->ChannelCount, Stream->BitDepth);

  alBufferData(buffer, format, HostBuffer.data(),
               samplesRead * Stream->BytesPerSample(), Stream->SampleRate);
  alSourceQueueBuffers(Source, 1, &buffer);
}

}  // namespace OpenAL
}  // namespace Audio
}  // namespace Impacto