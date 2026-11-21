#include "openalaudiochannel.h"
#include "../audiosystem.h"
#include "../audiostream.h"
#include "../controlleraudio.h"
#include "../../profile/configsystem.h"

#include <algorithm>

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

  CreateSource();
}

void OpenALAudioChannel::Reinit() { CreateSource(); }

OpenALAudioChannel::~OpenALAudioChannel() {
  if (Output == AO_Controller) {
    ControllerAudio::StopPlayback();
  }
  alDeleteSources(1, &Source);
  alDeleteBuffers((ALsizei)AudioBuffers.size(), AudioBuffers.data());
}

void OpenALAudioChannel::CreateSource() {
  alGenBuffers((ALsizei)AudioBuffers.size(), AudioBuffers.data());
  alGenSources(1, &Source);

  alSourcef(Source, AL_PITCH, 1);
  alSource3f(Source, AL_POSITION, 0, 0, 0);
  alSource3f(Source, AL_VELOCITY, 0, 0, 0);
  alSourcei(Source, AL_LOOPING, AL_FALSE);
  UpdateGain();
}

void OpenALAudioChannel::SetOutput(AudioOutputId output) {
  if (output == Output) return;

  const bool wasPlaybackStarted = PlaybackStarted;
  const size_t position =
      (Stream && wasPlaybackStarted) ? PlaybackPosition() : 0;

  if (Output == AO_Controller) {
    ControllerAudio::StopPlayback();
  } else {
    alSourceStop(Source);
    ALint queuedBuffers = 0;
    alGetSourcei(Source, AL_BUFFERS_QUEUED, &queuedBuffers);
    UnqueueBuffers(queuedBuffers);
  }

  Output = output;
  FirstQueuedBufferIndex = 0;
  BufferStartPositions.fill(0);
  PlaybackStarted = false;
  ControllerStartSample = 0;
  ControllerStreamFinished = false;
  if (!Stream || !wasPlaybackStarted) return;

  int samplePosition = (int)position;
  if (Looping && samplePosition >= Stream->LoopEnd) {
    samplePosition = Stream->LoopStart + (samplePosition - Stream->LoopEnd);
  }
  if (!Looping && Stream->Duration >= 0 && samplePosition >= Stream->Duration) {
    EndPlayback();
    return;
  }
  Stream->Seek(samplePosition);
  if (Output == AO_Controller) {
    ControllerStartSample = (size_t)samplePosition;
  }
}

void OpenALAudioChannel::Play(std::unique_ptr<AudioStream> stream, bool loop,
                              float fadeInDuration) {
  EndPlayback();

  Stream = std::move(stream);
  FadeDuration = fadeInDuration;
  FadeProgress = FadeDuration == 0 ? 1.0f : 0.0f;
  State = FadeDuration == 0 ? ACS_Playing : ACS_FadingIn;
  Looping = loop;

  AudioSubtitlesStart(this);
}

void OpenALAudioChannel::Stop(float fadeOutDuration) {
  if (State == ACS_Stopped) return;

  if (fadeOutDuration == 0 || State == ACS_Paused) {
    EndPlayback();
    return;
  }

  const float baseGain = BaseGain();
  FadeStartFactor = baseGain > 0.0f ? CurrentGain / baseGain : 0.0f;
  State = ACS_FadingOut;
  FadeDuration = fadeOutDuration;
  FadeProgress = 0;
}

void OpenALAudioChannel::EndPlayback() {
  if (Output == AO_Controller) {
    ControllerAudio::StopPlayback();
  }
  alSourceStop(Source);

  ALint queuedBuffers;
  alGetSourcei(Source, AL_BUFFERS_QUEUED, &queuedBuffers);
  UnqueueBuffers(queuedBuffers);

  Stream = nullptr;
  PlaybackStarted = false;
  ControllerStartSample = 0;
  ControllerStreamFinished = false;
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

size_t OpenALAudioChannel::PlaybackPosition() const {
  if (Output == AO_Controller) {
    if (!Stream || !PlaybackStarted) return ControllerStartSample;
    const uint64_t played32k = ControllerAudio::GetPlayedSamples32k();
    size_t pos =
        ControllerStartSample +
        static_cast<size_t>(
            (played32k * static_cast<uint64_t>(Stream->SampleRate)) / 32000ULL);
    if (!Looping && Stream->Duration >= 0) {
      pos = std::min(pos, static_cast<size_t>(Stream->Duration));
    }
    return pos;
  }

  int offset;
  alGetSourcei(Source, AL_SAMPLE_OFFSET, &offset);

  return BufferStartPositions[FirstQueuedBufferIndex] + offset;
}

float OpenALAudioChannel::PositionInSeconds() const {
  if (Stream == nullptr) return 0.0f;

  return (float)PlaybackPosition() / Stream->SampleRate;
}

void OpenALAudioChannel::Update(float dt) {
  UpdateFade(dt);

  if (Output == AO_Controller) {
    UpdateControllerPlayback();
    return;
  }

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
  ALint alState;
  ALint queued;
  alGetSourcei(Source, AL_SOURCE_STATE, &alState);
  alGetSourcei(Source, AL_BUFFERS_QUEUED, &queued);

  // Stop playback when OpenAL finished processing but not when there's buffer
  // underrun
  if (alState == AL_STOPPED && queued == 0 && PlaybackStarted) {
    EndPlayback();
    return;
  }

  // Only propagate the latest state to OpenAL
  // when the audio channel is updated to avoid "blips"
  // when multiple state changes occur in one frame
  if (alState == AL_PLAYING && State == ACS_Paused) {
    alSourcePause(Source);
  } else if (alState != AL_PLAYING &&
             (State == ACS_Playing || State == ACS_FadingIn ||
              State == ACS_FadingOut)) {
    alSourcePlay(Source);
    PlaybackStarted = true;
  } else if ((alState == AL_PLAYING || alState == AL_PAUSED) &&
             State == ACS_Stopped) {
    EndPlayback();
  }
}

void OpenALAudioChannel::UpdateControllerPlayback() {
  if (State == ACS_Stopped) {
    if (PlaybackStarted || Stream) EndPlayback();
    return;
  }

  if (State == ACS_Paused) {
    ControllerAudio::SetPaused(true);
    return;
  }

  UpdateGain();
  ControllerAudio::SetPaused(false);
  if (!Stream) {
    EndPlayback();
    return;
  }

  if (!PlaybackStarted) {
    ControllerStartSample = static_cast<size_t>(Stream->ReadPosition);
    ControllerStreamFinished = false;
    PlaybackStarted = true;
  }

  const int bytesPerSample = Stream->BytesPerSample();
  const int chunkSamples =
      std::min(AudioBufferSize / bytesPerSample, Stream->SampleRate / 20);

  while (!ControllerStreamFinished &&
         ControllerAudio::GetQueuedDurationMs() < ControllerTargetQueueMs) {
    const int samplesRead = ReadStreamSamples(chunkSamples);
    if (samplesRead > 0) {
      ControllerAudio::QueuePcm(HostBuffer.data(), samplesRead,
                                Stream->SampleRate, Stream->ChannelCount,
                                Stream->BitDepth);
    }

    if ((!Looping && Stream->ReadPosition >= Stream->Duration) ||
        samplesRead == 0) {
      ControllerStreamFinished = true;
      ControllerAudio::FinishQueue();
      break;
    }
  }

  if (ControllerStreamFinished && ControllerAudio::IsFinished()) {
    EndPlayback();
  }
}

float OpenALAudioChannel::BaseGain() const {
  const float groupVolume = Output == AO_Controller
                                ? Profile::ConfigSystem::SpeakerVolume
                                : GroupVolumes[Group];
  return MasterVolume * groupVolume * Volume;
}

// TODO what easing functions do we want for this?
void OpenALAudioChannel::UpdateGain() {
  float gain = BaseGain();
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
      gain *= FadeStartFactor * (1.0f - powf(FadeProgress, 3.0f));
      break;
  }

  CurrentGain = gain;
  if (Output == AO_Controller) {
    ControllerAudio::SetGain(gain);
  } else {
    alSourcef(Source, AL_GAIN, gain);
  }
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

  if (queuedBuffers >= std::ssize(AudioBuffers)) return false;

  return Looping || Stream->ReadPosition < Stream->Duration;
}

int OpenALAudioChannel::ReadStreamSamples(int maxSamples) {
  const int bytesPerSample = Stream->BytesPerSample();
  int samplesRead = 0;

  while (samplesRead < maxSamples &&
         (Looping || Stream->ReadPosition < Stream->Duration)) {
    int end = Looping ? Stream->LoopEnd : Stream->Duration;
    int samplesToRead =
        std::min(maxSamples - samplesRead, end - Stream->ReadPosition);
    if (samplesToRead <= 0) break;

    uint8_t* dest = &HostBuffer[samplesRead * bytesPerSample];
    int samplesReadThisIteration = Stream->Read(dest, samplesToRead);
    samplesRead += samplesReadThisIteration;

    if (Looping && Stream->ReadPosition >= Stream->LoopEnd) {
      Stream->Seek(Stream->LoopStart);
    }

    if (samplesReadThisIteration == 0) break;
  }

  return samplesRead;
}

void OpenALAudioChannel::QueueBuffer(size_t bufferIndex) {
  BufferStartPositions[bufferIndex] = Stream->ReadPosition;

  int maxSamples = AudioBufferSize / Stream->BytesPerSample();
  int samplesRead = ReadStreamSamples(maxSamples);

  ALuint buffer = AudioBuffers[bufferIndex];
  ALenum format = ToALFormat(Stream->ChannelCount, Stream->BitDepth);

  alBufferData(buffer, format, HostBuffer.data(),
               samplesRead * Stream->BytesPerSample(), Stream->SampleRate);
  alSourceQueueBuffers(Source, 1, &buffer);
}

}  // namespace OpenAL
}  // namespace Audio
}  // namespace Impacto