#include "openalaudiochannel.h"
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

  throw std::invalid_argument(fmt::format(
      "Unimplemented AL format with {} channels and a bit depth of {}",
      channels, bitdepth));
}

OpenALAudioChannel::OpenALAudioChannel(AudioChannelId id,
                                       AudioChannelGroup group)
    : AudioChannel(id, group),
      HostBuffer(std::vector(AudioBufferSize, (uint8_t)0)) {
  BufferStartPositions.fill(0);

  alGenBuffers((ALsizei)AudioBuffers.size(), AudioBuffers.data());
  alGenSources(1, &Source);

  alSourcef(Source, AL_PITCH, 1);
  alSource3f(Source, AL_POSITION, 0, 0, 0);
  alSource3f(Source, AL_VELOCITY, 0, 0, 0);
  alSourcei(Source, AL_LOOPING, AL_FALSE);
  UpdateGain();
}

OpenALAudioChannel::~OpenALAudioChannel() {
  alDeleteSources(1, &Source);
  alDeleteBuffers((ALsizei)AudioBuffers.size(), AudioBuffers.data());
}

void OpenALAudioChannel::Play(std::unique_ptr<AudioStream> stream, bool loop,
                              float fadeInDuration) {
  EndPlayback();

  Stream = std::move(stream);
  FadeDuration = fadeInDuration;
  FadeProgress = FadeDuration == 0 ? 1.0f : 0.0f;
  State = FadeDuration == 0 ? ACS_Playing : ACS_FadingIn;
  Looping = loop;
}

void OpenALAudioChannel::Stop(float fadeOutDuration) {
  if (State == ACS_Stopped) return;

  if (fadeOutDuration == 0 || State == ACS_Paused) {
    EndPlayback();
    return;
  }

  State = ACS_FadingOut;
  FadeDuration = fadeOutDuration;
  FadeProgress = 0;
}

void OpenALAudioChannel::EndPlayback() {
  alSourceStop(Source);

  ALint queuedBuffers;
  alGetSourcei(Source, AL_BUFFERS_QUEUED, &queuedBuffers);
  UnqueueBuffers(queuedBuffers);

  Stream = nullptr;
  PlaybackStarted = false;
  State = ACS_Stopped;
  FadeProgress = 0;
  FirstQueuedBufferIndex = 0;
  BufferStartPositions.fill(0);
}

void OpenALAudioChannel::Pause() {
  if (State != ACS_Playing && State != ACS_FadingIn && State != ACS_FadingOut) {
    return;
  }

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
  UpdateFade(dt);

  if (State == ACS_Playing || State == ACS_FadingIn || State == ACS_FadingOut) {
    // Continuously update the gain
    // because the global state might have changed
    UpdateGain();

    ALint processedBuffers;
    alGetSourcei(Source, AL_BUFFERS_PROCESSED, &processedBuffers);
    UnqueueBuffers(processedBuffers);

    // TODO do this on background thread
    while (CanQueueBuffer()) {
      int queuedBuffers;
      alGetSourcei(Source, AL_BUFFERS_QUEUED, &queuedBuffers);

      size_t firstFreeIndex =
          (FirstQueuedBufferIndex + queuedBuffers) % AudioBuffers.size();
      QueueBuffer(firstFreeIndex);

      if (queuedBuffers == 0 && PlaybackStarted) {
        ImpLog(LogLevel::Error, LogChannel::Audio,
               "Buffer underrun on channel {:d}\n", Id);

        // Restart playback in case of buffer underrun
        alSourcePlay(Source);
      }
    }
  }

  UpdatePlayback();
}

void OpenALAudioChannel::UpdateFade(float dt) {
  if (State != ACS_FadingIn && State != ACS_FadingOut) return;

  FadeProgress = std::min(1.0f, FadeProgress + dt / FadeDuration);
  if (FadeProgress >= 1.0f) {
    State = State == ACS_FadingIn ? ACS_Playing : ACS_Stopped;
  }
}

void OpenALAudioChannel::UpdatePlayback() {
  ALenum alState;
  alGetSourcei(Source, AL_SOURCE_STATE, &alState);

  // Stop playback when OpenAL finished processing the whole stream
  if (alState == AL_STOPPED && PlaybackStarted) {
    EndPlayback();
    return;
  }

  // Only propogate the latest state to OpenAL
  // when the audio channel is updated to avoid "blips"
  // when multiple state changes occur in one frame
  if (alState != AL_PAUSED && State == ACS_Paused) {
    alSourcePause(Source);
  } else if (alState != AL_PLAYING &&
             (State == ACS_Playing || State == ACS_FadingIn ||
              State == ACS_FadingOut)) {
    alSourcePlay(Source);
    PlaybackStarted = true;
  }
}

// TODO what easing functions do we want for this?
void OpenALAudioChannel::UpdateGain() {
  float gain = MasterVolume * GroupVolumes[Group] * Volume;
  switch (State) {
    case ACS_Stopped:
      return;
    case ACS_Paused:
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
    size_t bufferIndex = FirstQueuedBufferIndex;
    alSourceUnqueueBuffers(Source, 1, &AudioBuffers[bufferIndex]);
    FirstQueuedBufferIndex = (bufferIndex + 1) % AudioBuffers.size();
  }
}

bool OpenALAudioChannel::CanQueueBuffer() {
  ALint queuedBuffers;
  alGetSourcei(Source, AL_BUFFERS_QUEUED, &queuedBuffers);

  if ((size_t)queuedBuffers >= AudioBuffers.size()) return false;

  return Looping || Stream->ReadPosition < Stream->Duration;
}

void OpenALAudioChannel::QueueBuffer(size_t bufferIndex) {
  BufferStartPositions[bufferIndex] = Stream->ReadPosition;

  int maxSamples = AudioBufferSize / Stream->BytesPerSample();
  int samplesRead = 0;

  while (samplesRead < maxSamples && Stream->ReadPosition < Stream->Duration) {
    int end = Looping ? Stream->LoopEnd : Stream->Duration;
    int samplesToRead =
        std::min(maxSamples - samplesRead, end - Stream->ReadPosition);

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