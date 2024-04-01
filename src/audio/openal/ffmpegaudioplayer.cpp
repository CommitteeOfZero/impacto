#include "ffmpegaudioplayer.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavformat/avformat.h>
};

namespace Impacto {
namespace Audio {
namespace OpenAL {

void FFmpegAudioPlayer::Init() {
  alGenSources(1, &ALSource);
  alSourcef(ALSource, AL_PITCH, 1);
  alSourcef(ALSource, AL_GAIN, 1);
  alSource3f(ALSource, AL_POSITION, 0, 0, 0);
  alSource3f(ALSource, AL_VELOCITY, 0, 0, 0);
  alSourcei(ALSource, AL_LOOPING, AL_FALSE);

  alGenBuffers(AudioBufferCount, BufferIds);

  AudioClock = new Video::Clock();
}

void FFmpegAudioPlayer::InitConvertContext(AVCodecContext* codecCtx) {
  AudioConvertContext =
      swr_alloc_set_opts(NULL, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16,
                         codecCtx->sample_rate, AV_CH_LAYOUT_STEREO,
                         codecCtx->sample_fmt, codecCtx->sample_rate, 0, NULL);
  swr_init(AudioConvertContext);
  av_samples_alloc_array_and_samples(&AudioBuffer, &AudioLinesize,
                                     codecCtx->channels, 32000,
                                     AV_SAMPLE_FMT_S16, 0);
}

void FFmpegAudioPlayer::Stop() { alSourceStop(ALSource); }

void FFmpegAudioPlayer::Unload() {
  if (AudioBuffer) av_free(AudioBuffer);
  AudioBuffer = 0;
  alSourcei(ALSource, AL_BUFFER, 0);
  Stop();
  alDeleteSources(1, &ALSource);
  alGenSources(1, &ALSource);
  First = true;
}

void FFmpegAudioPlayer::FillAudioBuffers() {
  while (FreeBufferCount && !Player->AbortRequest) {
    int totalSize = 0;

    do {
      bool firstFrame = true;

      SDL_LockMutex(Player->AudioStream->FrameLock);
      if (Player->AudioStream->FrameQueue.empty()) {
        SDL_UnlockMutex(Player->AudioStream->FrameLock);
        SDL_CondSignal(Player->AudioStream->DecodeCond);
        continue;
      }
      auto aFrame = Player->AudioStream->FrameQueue.front();
      SDL_UnlockMutex(Player->AudioStream->FrameLock);

      if (aFrame.Serial == INT32_MIN) {
        SDL_LockMutex(Player->AudioStream->FrameLock);
        Player->AudioStream->FrameQueue.pop();
        SDL_UnlockMutex(Player->AudioStream->FrameLock);
        break;
      }

      if (firstFrame) {
        BufferStartPositions[FirstFreeBuffer] =
            aFrame.Frame->best_effort_timestamp *
            av_q2d(Player->AudioStream->stream->time_base) *
            aFrame.Frame->sample_rate;
        firstFrame = false;
      }

      int64_t delay =
          swr_get_delay(AudioConvertContext, aFrame.Frame->sample_rate);
      int64_t samplesPerCh = av_rescale_rnd(
          (int64_t)aFrame.Frame->nb_samples + delay, aFrame.Frame->sample_rate,
          aFrame.Frame->sample_rate, AV_ROUND_UP);
      int outputSamples =
          swr_convert(AudioConvertContext, AudioBuffer, samplesPerCh,
                      (const uint8_t**)aFrame.Frame->extended_data,
                      aFrame.Frame->nb_samples);

      int bufferSize = av_samples_get_buffer_size(NULL, 2, outputSamples,
                                                  AV_SAMPLE_FMT_S16, 1);
      memcpy(&HostBuffer[totalSize], AudioBuffer[0], bufferSize);
      totalSize += bufferSize;

      av_frame_free(&aFrame.Frame);
      SDL_LockMutex(Player->AudioStream->FrameLock);
      Player->AudioStream->FrameQueue.pop();
      SDL_UnlockMutex(Player->AudioStream->FrameLock);
      SDL_CondSignal(Player->AudioStream->DecodeCond);
    } while (totalSize <= 4096);

    alSourceUnqueueBuffers(ALSource, 1, &BufferIds[FirstFreeBuffer]);
    FreeBufferCount--;

    alBufferData(BufferIds[FirstFreeBuffer], AL_FORMAT_STEREO16, HostBuffer,
                 totalSize, Player->AudioStream->CodecContext->sample_rate);

    alSourceQueueBuffers(ALSource, 1, &BufferIds[FirstFreeBuffer]);

    FirstFreeBuffer++;
    FirstFreeBuffer %= AudioBufferCount;
  }
}

void FFmpegAudioPlayer::Process() {
  float gain = Audio::MasterVolume * Audio::GroupVolumes[Audio::ACG_Movie];
  alSourcef(ALSource, AL_GAIN, gain);
  SDL_LockMutex(Player->AudioStream->FrameLock);
  ImpLogSlow(LL_Trace, LC_Video, "AudioStream->FrameQueue.size() = %d\n",
             Player->AudioStream->FrameQueue.size());
  if (!Player->AudioStream->FrameQueue.empty()) {
    SDL_UnlockMutex(Player->AudioStream->FrameLock);
    alGetSourcei(ALSource, AL_BUFFERS_PROCESSED, &FreeBufferCount);
    if (First) {
      FreeBufferCount = AudioBufferCount;
      First = false;
    }

    int currentlyPlayingBuffer =
        (FirstFreeBuffer + FreeBufferCount) % AudioBufferCount;

    int offset;
    alGetSourcei(ALSource, AL_SAMPLE_OFFSET, &offset);
    int samplePosition = BufferStartPositions[currentlyPlayingBuffer] + offset;
    samplePosition = std::min(samplePosition, Player->AudioStream->Duration);
    double position =
        samplePosition / (double)Player->AudioStream->CodecContext->sample_rate;
    ImpLogSlow(LL_Trace, LC_Video, "samplePosition: %f\n", position);

    AudioClock->Set(position, 0);

    FillAudioBuffers();
    ALint sourceState;
    alGetSourcei(ALSource, AL_SOURCE_STATE, &sourceState);
    if (sourceState == AL_STOPPED || sourceState == AL_INITIAL) {
      alSourcePlay(ALSource);
    }
  } else {
    SDL_UnlockMutex(Player->AudioStream->FrameLock);
    SDL_CondSignal(Player->AudioStream->DecodeCond);
    ImpLog(LL_Debug, LC_Video, "Ran out of audio frames!\n");
  }
}

}  // namespace OpenAL
}  // namespace Audio
}  // namespace Impacto
