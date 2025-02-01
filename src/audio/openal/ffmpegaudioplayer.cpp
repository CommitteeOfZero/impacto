#include "ffmpegaudioplayer.h"
#include "../../video/ffmpegplayer.h"
#include "../audiosystem.h"
#include "al.h"
#include <mutex>
#include <audioresampler.h>
#include <timestamp.h>

extern "C" {
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavformat/avformat.h>
};

namespace Impacto {
namespace Audio {
namespace OpenAL {

FFmpegAudioPlayer::~FFmpegAudioPlayer() {
  alSourceStop(ALSource);
  av_freep(&AudioBuffer[0]);
  swr_free(&AudioConvertContext);
  alDeleteBuffers(AudioBufferCount, BufferIds);
  alDeleteSources(1, &ALSource);
}

void FFmpegAudioPlayer::Init() {
  alGenSources(1, &ALSource);
  alSourcef(ALSource, AL_PITCH, 1);
  alSourcef(ALSource, AL_GAIN, 1);
  alSource3f(ALSource, AL_POSITION, 0, 0, 0);
  alSource3f(ALSource, AL_VELOCITY, 0, 0, 0);
  alSourcei(ALSource, AL_LOOPING, AL_FALSE);

  alGenBuffers(AudioBufferCount, BufferIds);
}

void FFmpegAudioPlayer::InitConvertContext(AVCodecContext* codecCtx) {
  AVChannelLayout stereoFormat;
  av_channel_layout_default(&stereoFormat, 2);
  int res = swr_alloc_set_opts2(&AudioConvertContext, &stereoFormat,
                                AV_SAMPLE_FMT_S16, codecCtx->sample_rate,
                                &stereoFormat, codecCtx->sample_fmt,
                                codecCtx->sample_rate, 0, NULL);
  if (res) {
    ImpLog(LL_Error, LC_Video, "Could not create audio convert context!\n");
    return;
  }

  swr_init(AudioConvertContext);
  av_samples_alloc_array_and_samples(&AudioBuffer, &AudioLinesize,
                                     codecCtx->ch_layout.nb_channels, 32000,
                                     AV_SAMPLE_FMT_S16, 0);
}

void FFmpegAudioPlayer::Stop() { alSourceStop(ALSource); }

void FFmpegAudioPlayer::Unload() {
  av_freep(&AudioBuffer[0]);
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

      std::unique_lock lock(Player->AudioStream->FrameLock);
      if (Player->AudioStream->FrameQueue.empty()) {
        Player->AudioStream->DecodeCond.notify_one();
        continue;
      }
      Video::AVFrameItem<AVMEDIA_TYPE_AUDIO> aFrame =
          std::move(Player->AudioStream->FrameQueue.front());

      if (aFrame.Serial == INT32_MIN) {
        Player->AudioStream->FrameQueue.pop();
        break;
      }
      Player->AudioStream->FrameQueue.pop();
      Player->AudioStream->DecodeCond.notify_one();

      if (firstFrame) {
        BufferStartPositions[FirstFreeBuffer] =
            (int)(aFrame.Frame.pts().timestamp() * aFrame.Frame.sampleRate() *
                  Player->AudioStream->stream.timeBase().getNumerator() /
                  Player->AudioStream->stream.timeBase().getDenominator());
        firstFrame = false;
      }

      int64_t delay =
          swr_get_delay(AudioConvertContext, aFrame.Frame.sampleRate());
      int64_t samplesPerCh = av_rescale_rnd(
          (int64_t)aFrame.Frame.samplesCount() + delay,
          aFrame.Frame.sampleRate(), aFrame.Frame.sampleRate(), AV_ROUND_UP);
      int outputSamples =
          swr_convert(AudioConvertContext, AudioBuffer, (int)samplesPerCh,
                      (const uint8_t**)aFrame.Frame.raw()->extended_data,
                      aFrame.Frame.samplesCount());

      int bufferSize = av_samples_get_buffer_size(NULL, 2, outputSamples,
                                                  AV_SAMPLE_FMT_S16, 1);
      memcpy(&HostBuffer[totalSize], AudioBuffer[0], bufferSize);
      totalSize += bufferSize;
    } while (totalSize <= 4096);

    alSourceUnqueueBuffers(ALSource, 1, &BufferIds[FirstFreeBuffer]);
    FreeBufferCount--;

    alBufferData(BufferIds[FirstFreeBuffer], AL_FORMAT_STEREO16, HostBuffer,
                 totalSize, Player->AudioStream->CodecContext.sampleRate());

    alSourceQueueBuffers(ALSource, 1, &BufferIds[FirstFreeBuffer]);

    FirstFreeBuffer++;
    FirstFreeBuffer %= AudioBufferCount;
  }
}

void FFmpegAudioPlayer::Process() {
  float gain = Audio::MasterVolume * Audio::GroupVolumes[Audio::ACG_Movie];
  alSourcef(ALSource, AL_GAIN, gain);
  Player->AudioStream->FrameLock.lock();
  ImpLogSlow(LL_Trace, LC_Video, "AudioStream->FrameQueue.size() = %d\n",
             Player->AudioStream->FrameQueue.size());
  if (!Player->AudioStream->FrameQueue.empty()) {
    Player->AudioStream->FrameLock.unlock();
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
    int sampleRate = Player->AudioStream->CodecContext.sampleRate();
    samplePosition = std::min(samplePosition, Player->AudioStream->Duration);
    auto audioTime = av::Timestamp(samplePosition, av::Rational(1, sampleRate));
    double audioS = audioTime.seconds();
    ImpLogSlow(LL_Trace, LC_Video, "samplePosition: %f\n", audioS);
    AudioClock.Set(audioS, 0);

    FillAudioBuffers();
    ALint sourceState;
    alGetSourcei(ALSource, AL_SOURCE_STATE, &sourceState);
    if (sourceState == AL_STOPPED || sourceState == AL_INITIAL) {
      alSourcePlay(ALSource);
    }
  } else {
    Player->AudioStream->FrameLock.unlock();
    Player->AudioStream->DecodeCond.notify_one();
    ImpLog(LL_Debug, LC_Video, "Ran out of audio frames!\n");
  }
}

}  // namespace OpenAL
}  // namespace Audio
}  // namespace Impacto
