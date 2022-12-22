#include "videoplayer.h"
#include "../log.h"
#include <utility>
#include "../profile/game.h"
#include "../audio/audiosystem.h"

namespace Impacto {
namespace Video {

int StreamRead(void* ptr, uint8_t* buf, int buf_size) {
  Io::InputStream* stream = reinterpret_cast<Io::InputStream*>(ptr);

  uint64_t bytesRead = stream->Read(buf, buf_size);
  if ((bytesRead == IoError_Fail || bytesRead == IoError_Eof))
    return AVERROR_EOF;

  return bytesRead;
}

int64_t StreamSeek(void* ptr, int64_t pos, int origin) {
  Io::InputStream* stream = reinterpret_cast<Io::InputStream*>(ptr);

  if (origin == AVSEEK_SIZE) return stream->Meta.Size;
  int64_t newPos = stream->Seek(pos, origin);
  if ((newPos == IoError_Fail) || (newPos == IoError_Eof)) return AVERROR_EOF;

  return newPos;
}

static int PlayerRead(void* ptr) {
  ((VideoPlayer*)ptr)->Read();
  return 0;
}

static int PlayerDecodeVideo(void* ptr) {
  ((VideoPlayer*)ptr)->Decode(AVMEDIA_TYPE_VIDEO);
  return 0;
}

static int PlayerDecodeAudio(void* ptr) {
  ((VideoPlayer*)ptr)->Decode(AVMEDIA_TYPE_AUDIO);
  return 0;
}

Clock::Clock() {
  Speed = 1.0;
  Paused = false;
  Pts = NAN;
  LastUpdated = av_gettime_relative() / 1000000.0;
  PtsDrift = Pts - LastUpdated;
  Serial = -1;
}

void Clock::SyncTo(Clock* target) {
  double clock = Get();
  double targetClock = target->Get();
  if (!isnan(targetClock) && (isnan(clock) || fabs(clock - targetClock) > 10.0))
    Set(targetClock, target->Serial);
}

void Clock::Set(double pts, int serial) {
  double time = av_gettime_relative() / 1000000.0;
  Pts = pts;
  LastUpdated = time;
  PtsDrift = Pts - time;
  Serial = serial;
}

double Clock::Get() {
  if (Paused) {
    return Pts;
  } else {
    double time = av_gettime_relative() / 1000000.0;
    return PtsDrift + time - (time - LastUpdated) * (1.0 - Speed);
  }
}

VideoPlayer::~VideoPlayer() { IsInit = false; }

void VideoPlayer::Init() {
  assert(IsInit == false);

  alGenSources(1, &ALSource);
  alSourcef(ALSource, AL_PITCH, 1);
  alSourcef(ALSource, AL_GAIN, 1);
  alSource3f(ALSource, AL_POSITION, 0, 0, 0);
  alSource3f(ALSource, AL_VELOCITY, 0, 0, 0);
  alSourcei(ALSource, AL_LOOPING, AL_FALSE);

  alGenBuffers(AudioBufferCount, BufferIds);

  ReadCond = SDL_CreateCond();

  IsInit = true;
}

void VideoPlayer::Play(Io::InputStream* stream, bool looping, bool alpha) {
  // Don't do anything if we don't have the video system
  if (!IsInit) return;
  AbortRequest = false;
  SeekRequest = false;
  Looping = looping;
  IsAlpha = alpha;
  ImpLog(LL_Info, LC_Video, "Opening file: %s from: %s\n",
         stream->Meta.FileName.c_str(), stream->Meta.ArchiveFileName.c_str());

  int videoStreamId = -1;
  int audioStreamId = -1;
  FileStreamBuffer = (uint8_t*)av_malloc(FILESTREAMBUFFERSZ);
  if (FileStreamBuffer == nullptr) {
    ImpLog(LL_Error, LC_Video, "Error allocating buffer!\n");
    return;
  }
  IoContext = avio_alloc_context(FileStreamBuffer, FILESTREAMBUFFERSZ, 0,
                                 stream, StreamRead, 0, StreamSeek);

  FormatContext = avformat_alloc_context();
  FormatContext->pb = IoContext;
  FormatContext->flags |= AVFMT_FLAG_CUSTOM_IO;

  if (avformat_open_input(&FormatContext, "", nullptr, nullptr) < 0) {
    ImpLog(LL_Error, LC_Video, "Error opening file!\n");
    return;
  }

  avformat_find_stream_info(FormatContext, nullptr);

  videoStreamId = av_find_best_stream(FormatContext, AVMEDIA_TYPE_VIDEO, -1, -1,
                                      nullptr, 0);
  audioStreamId = av_find_best_stream(FormatContext, AVMEDIA_TYPE_AUDIO, -1,
                                      videoStreamId, nullptr, 0);
  AVStream* videoStream = FormatContext->streams[videoStreamId];
  AVStream* audioStream = NULL;
  if (audioStreamId != AVERROR_STREAM_NOT_FOUND)
    audioStream = FormatContext->streams[audioStreamId];

  for (int j = 0; j < FormatContext->nb_streams; ++j) {
    FormatContext->streams[j]->discard =
        j == videoStreamId || j == audioStreamId ? AVDISCARD_DEFAULT
                                                 : AVDISCARD_ALL;
  }

  AVCodec* codec = NULL;
  // Find and open the video codec
  if (videoStreamId != AVERROR_STREAM_NOT_FOUND) {
    codec = avcodec_find_decoder(videoStream->codecpar->codec_id);
    if (codec == NULL) {
      ImpLog(LL_Error, LC_Video, "Unsupported codec!\n");
      return;
    }

    AVCodecContext* codecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecCtx, videoStream->codecpar);
    AVDictionary* options = NULL;
    av_dict_set(&options, "threads", "auto", 0);

    avcodec_open2(codecCtx, codec, &options);
    VideoStream = new FFmpegStream(videoStream, codecCtx);
    VideoTexture.Init(codecCtx->width, codecCtx->height);
  }

  // Find and open the audio codec
  if (audioStreamId != AVERROR_STREAM_NOT_FOUND) {
    codec = avcodec_find_decoder(audioStream->codecpar->codec_id);
    if (codec == NULL) {
      ImpLog(LL_Error, LC_Video, "Unsupported codec!\n");
      return;
    }

    AVCodecContext* codecCtx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codecCtx, audioStream->codecpar);
    AVDictionary* options = NULL;
    av_dict_set(&options, "threads", "auto", 0);

    avcodec_open2(codecCtx, codec, &options);
    AudioStream = new FFmpegStream(audioStream, codecCtx);
    AudioStream->Duration = audioStream->duration *
                            av_q2d(audioStream->time_base) *
                            codecCtx->sample_rate;

    AudioConvertContext = swr_alloc_set_opts(
        NULL, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, codecCtx->sample_rate,
        AV_CH_LAYOUT_STEREO, codecCtx->sample_fmt, codecCtx->sample_rate, 0,
        NULL);
    swr_init(AudioConvertContext);
    av_samples_alloc_array_and_samples(&AudioBuffer, &AudioLinesize,
                                       codecCtx->channels, 32000,
                                       AV_SAMPLE_FMT_S16, 0);
  }

  VideoClock = new Clock();
  ExternalClock = new Clock();
  AudioClock = new Clock();

  MasterClock = ExternalClock;

  MaxFrameDuration =
      (FormatContext->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0;

  // Danger zone
  ReadThreadID = SDL_CreateThread(&PlayerRead, "videoplayer::read", this);
  if (VideoStream) {
    VideoStream->DecoderThreadID =
        SDL_CreateThread(&PlayerDecodeVideo, "videoplayer::decodevideo", this);
  }
  if (AudioStream) {
    MasterClock = AudioClock;
    AudioStream->DecoderThreadID =
        SDL_CreateThread(&PlayerDecodeAudio, "videoplayer::decodeaudio", this);
  }

  IsPlaying = true;
}

bool VideoPlayer::QueuesHaveEnoughPackets() {
  SDL_LockMutex(VideoStream->PacketLock);
  int videoQueueSize = VideoStream->PacketQueue.size();
  SDL_UnlockMutex(VideoStream->PacketLock);
  int audioQueueSize = 0;
  if (AudioStream) {
    SDL_LockMutex(AudioStream->PacketLock);
    audioQueueSize = AudioStream->PacketQueue.size();
    SDL_UnlockMutex(AudioStream->PacketLock);
  }

  return (videoQueueSize > 25 && audioQueueSize > 25);
}

void VideoPlayer::Read() {
  int serial = 0;
  AVPacket* packet = av_packet_alloc();
  SDL_mutex* waitMutex = SDL_CreateMutex();
  if (!packet) {
    ImpLog(LL_Error, LC_Video, "Failed to allocate a packet!\n");
    SDL_DestroyMutex(waitMutex);
    return;
  }
  while (!AbortRequest) {
    if (SeekRequest) {
      SeekRequest = false;

      int64_t timestamp = SeekPosition * AV_TIME_BASE;
      avformat_seek_file(FormatContext, -1, timestamp - 1 * AV_TIME_BASE,
                         timestamp, timestamp, 0);

      if (VideoStream) {
        VideoStream->FlushPacketQueue();
        VideoStream->FlushFrameQueue();
      }

      if (AudioStream) {
        AudioStream->FlushPacketQueue();
        AudioStream->FlushFrameQueue();
      }

      FrameTimer = 0.0;
      PreviousFrameTimestamp = -1;
    }

    if (QueuesHaveEnoughPackets() || ReaderEOF) {
      SDL_LockMutex(waitMutex);
      SDL_CondWaitTimeout(ReadCond, waitMutex, 10);
      SDL_UnlockMutex(waitMutex);
      continue;
    }

    int ret = av_read_frame(FormatContext, packet);
    if (ret >= 0) {
      if (packet->stream_index == VideoStream->stream->index) {
        AVPacketItem item;
        item.Packet = av_packet_alloc();
        av_packet_move_ref(item.Packet, packet);
        item.Serial = VideoStream->PacketQueueSerial;
        SDL_LockMutex(VideoStream->PacketLock);
        VideoStream->PacketQueue.push(item);
        SDL_UnlockMutex(VideoStream->PacketLock);
      } else if (packet->stream_index == AudioStream->stream->index) {
        AVPacketItem item;
        item.Packet = av_packet_alloc();
        av_packet_move_ref(item.Packet, packet);
        item.Serial = AudioStream->PacketQueueSerial;
        SDL_LockMutex(AudioStream->PacketLock);
        AudioStream->PacketQueue.push(item);
        SDL_UnlockMutex(AudioStream->PacketLock);
      } else {
        av_packet_unref(packet);
      }
    } else {
      if (ret == AVERROR_EOF) {
        ImpLog(LL_Debug, LC_Video, "EOF!\n");
        ReaderEOF = true;
        AVPacketItem item;
        item.Packet = av_packet_alloc();
        av_packet_move_ref(item.Packet, packet);
        item.Serial = INT32_MIN;
        if (AudioStream) {
          SDL_LockMutex(AudioStream->PacketLock);
          AudioStream->PacketQueue.push(item);
          SDL_UnlockMutex(AudioStream->PacketLock);
        }
        SDL_LockMutex(VideoStream->PacketLock);
        VideoStream->PacketQueue.push(item);
        SDL_UnlockMutex(VideoStream->PacketLock);
      } else {
        char error[255];
        av_strerror(ret, error, 255);
        ImpLog(LL_Error, LC_Video, "Uh oh %s\n", error);
      }
      av_packet_unref(packet);
    }
  }
}

void VideoPlayer::Decode(AVMediaType avType) {
  int ret = 0;
  AVFrame* frame = av_frame_alloc();
  SDL_mutex* waitMutex = SDL_CreateMutex();
  FFmpegStream* stream = 0;
  AVRational frameRate = {};
  double duration = 0.0;
  switch (avType) {
    case AVMEDIA_TYPE_VIDEO:
      stream = VideoStream;
      frameRate = av_guess_frame_rate(FormatContext, stream->stream, NULL);
      duration =
          (frameRate.num && frameRate.den ? av_q2d(frameRate) / 1000.0 : 0.0);
      break;
    case AVMEDIA_TYPE_AUDIO:
      stream = AudioStream;
      break;
  }

  while (!AbortRequest) {
    SDL_LockMutex(stream->PacketLock);
    ImpLogSlow(LL_Trace, LC_Video, "%d->PacketQueue.size() = %d\n", avType,
               stream->PacketQueue.size());
    if (stream->PacketQueue.empty()) {
      SDL_UnlockMutex(stream->PacketLock);
      SDL_CondSignal(ReadCond);
      ImpLogSlow(LL_Trace, LC_Video, "%d Decoder starving!\n", avType);
      continue;
    }
    SDL_UnlockMutex(stream->PacketLock);

    SDL_LockMutex(stream->FrameLock);
    if (stream->FrameQueue.size() > 60) {
      SDL_UnlockMutex(stream->FrameLock);

      ImpLogSlow(LL_Trace, LC_Video, "%d FrameQueue full!\n", avType);
      SDL_LockMutex(waitMutex);
      SDL_CondWait(stream->DecodeCond, waitMutex);
      SDL_UnlockMutex(waitMutex);
      continue;
    }
    SDL_UnlockMutex(stream->FrameLock);

    AVPacketItem packet;
    while (true) {
      int prevSerial = stream->CurrentPacketSerial;
      SDL_LockMutex(stream->PacketLock);
      packet = stream->PacketQueue.front();
      stream->CurrentPacketSerial = packet.Serial;
      if (prevSerial == INT32_MIN || stream->CurrentPacketSerial == INT32_MIN) {
        SDL_UnlockMutex(stream->PacketLock);
        break;
      }
      if (prevSerial != stream->CurrentPacketSerial) {
        avcodec_flush_buffers(stream->CodecContext);
        if (avType == AVMEDIA_TYPE_AUDIO) {
          alSourceStop(ALSource);
        }
      }
      if (stream->PacketQueueSerial == stream->CurrentPacketSerial) {
        SDL_UnlockMutex(stream->PacketLock);
        break;
      }
      av_packet_free(&packet.Packet);
      stream->PacketQueue.pop();
      SDL_UnlockMutex(stream->PacketLock);
    }

    do {
      ret = avcodec_receive_frame(stream->CodecContext, frame);
      if (ret >= 0) {
        AVFrameItem item;
        item.Serial = stream->PacketQueueSerial;
        item.Timestamp =
            frame->best_effort_timestamp * av_q2d(stream->stream->time_base);
        item.Duration = duration;
        item.Frame = av_frame_alloc();
        av_frame_move_ref(item.Frame, frame);
        SDL_LockMutex(stream->FrameLock);
        stream->FrameQueue.push(item);
        SDL_UnlockMutex(stream->FrameLock);
      }
      if (ret == AVERROR_EOF) {
        AVFrameItem item;
        item.Frame = av_frame_alloc();
        av_frame_move_ref(item.Frame, frame);
        item.Serial = INT32_MIN;
        SDL_LockMutex(stream->FrameLock);
        stream->FrameQueue.push(item);
        SDL_UnlockMutex(stream->FrameLock);
        avcodec_flush_buffers(stream->CodecContext);
        SDL_LockMutex(waitMutex);
        SDL_CondWait(stream->DecodeCond, waitMutex);
        SDL_UnlockMutex(waitMutex);
        continue;
      }
    } while (ret != AVERROR(EAGAIN));

    if (packet.Serial == INT32_MIN) {
      AVFrameItem item;
      item.Frame = 0;
      item.Serial = INT32_MIN;
      SDL_LockMutex(stream->FrameLock);
      stream->FrameQueue.push(item);
      SDL_UnlockMutex(stream->FrameLock);
      SDL_LockMutex(stream->PacketLock);
      stream->PacketQueue.pop();
      SDL_UnlockMutex(stream->PacketLock);
      stream->CurrentPacketSerial = -1;
      SDL_LockMutex(waitMutex);
      SDL_CondWait(stream->DecodeCond, waitMutex);
      SDL_UnlockMutex(waitMutex);
      continue;
    }
    SDL_LockMutex(stream->PacketLock);
    if (packet.Packet->data) {
      ret = avcodec_send_packet(stream->CodecContext, packet.Packet);
      if (ret >= 0) {
        av_packet_free(&packet.Packet);
        SDL_LockMutex(stream->PacketLock);
        stream->PacketQueue.pop();
        SDL_UnlockMutex(stream->PacketLock);
      }
    }
    SDL_UnlockMutex(stream->PacketLock);
  }
}

void VideoPlayer::Stop() {
  if (IsPlaying) {
    IsPlaying = false;
    PlaybackStarted = false;
    AbortRequest = true;
    SDL_WaitThread(ReadThreadID, NULL);
    ReaderEOF = false;
    if (AudioStream) {
      SDL_CondSignal(AudioStream->DecodeCond);
      SDL_WaitThread(AudioThreadID, NULL);
      SDL_WaitThread(AudioStream->DecoderThreadID, NULL);
      delete AudioStream;
      AudioStream = 0;
    }
    if (VideoStream) {
      SDL_CondSignal(VideoStream->DecodeCond);
      SDL_WaitThread(VideoStream->DecoderThreadID, NULL);
      delete VideoStream;
      VideoStream = 0;
    }
    if (AudioBuffer) av_free(AudioBuffer);
    AudioBuffer = 0;
    alSourcei(ALSource, AL_BUFFER, NULL);
    alSourceStop(ALSource);
    alDeleteSources(1, &ALSource);
    alGenSources(1, &ALSource);
    First = true;
    FrameTimer = 0.0;
    PreviousFrameTimestamp = 0.0;
    avformat_close_input(&FormatContext);
    if (IoContext)
      reinterpret_cast<Io::InputStream*>(IoContext->opaque)->~InputStream();
    avio_context_free(&IoContext);
  }
}

void VideoPlayer::Seek(int64_t pos) {
  SeekRequest = true;
  SeekPosition = pos;
  ReaderEOF = false;
  SDL_CondSignal(ReadCond);
}

void VideoPlayer::ProcessAudio() {
  float gain = Audio::MasterVolume * Audio::GroupVolumes[Audio::ACG_Movie];
  alSourcef(ALSource, AL_GAIN, gain);
  SDL_LockMutex(AudioStream->FrameLock);
  ImpLogSlow(LL_Trace, LC_Video, "AudioStream->FrameQueue.size() = %d\n",
             AudioStream->FrameQueue.size());
  if (!AudioStream->FrameQueue.empty()) {
    SDL_UnlockMutex(AudioStream->FrameLock);
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
    samplePosition = std::min(samplePosition, AudioStream->Duration);
    double position =
        samplePosition / (double)AudioStream->CodecContext->sample_rate;
    ImpLogSlow(LL_Trace, LC_Video, "samplePosition: %f\n", position);

    AudioClock->Set(position, 0);

    FillAudioBuffers();
    ALint sourceState;
    alGetSourcei(ALSource, AL_SOURCE_STATE, &sourceState);
    if (sourceState == AL_STOPPED || sourceState == AL_INITIAL) {
      alSourcePlay(ALSource);
    }
  } else {
    SDL_UnlockMutex(AudioStream->FrameLock);
    SDL_CondSignal(AudioStream->DecodeCond);
    ImpLog(LL_Debug, LC_Video, "Ran out of audio frames!\n");
  }
}

void VideoPlayer::Update(float dt) {
  if (IsPlaying) {
    if (AudioStream) ProcessAudio();
    while (true) {
      SDL_LockMutex(VideoStream->FrameLock);
      if (VideoStream->FrameQueue.empty()) {
        SDL_UnlockMutex(VideoStream->FrameLock);
        SDL_CondSignal(VideoStream->DecodeCond);
        break;
      }
      ImpLogSlow(LL_Trace, LC_Video, "VideoStream->FrameQueue.size() = %d\n",
                 VideoStream->FrameQueue.size());
      auto frame = VideoStream->FrameQueue.front();
      SDL_UnlockMutex(VideoStream->FrameLock);
      if (frame.Serial == INT32_MIN) {
        if (Looping) {
          Seek(0);
        } else {
          Stop();
        }
        return;
      }
      if (!FrameTimer) {
        FrameTimer = av_gettime_relative() / 1000000.0;
      }
      double time = av_gettime_relative() / 1000000.0;
      double duration;
      if (PreviousFrameTimestamp == -1) {
        duration = 0.0;
      } else {
        duration = frame.Timestamp - PreviousFrameTimestamp;
      }

      if (AudioStream) {
        duration = GetTargetDelay(duration);
      } else {
        duration = frame.Duration;
      }

      if (time < FrameTimer + duration) {
        break;
      }

      FrameTimer += duration;
      if (duration > 0 && time - FrameTimer > 0.1) {
        FrameTimer = time;
      }
      PreviousFrameTimestamp = frame.Timestamp;

      VideoClock->Set(frame.Timestamp, frame.Serial);
      ExternalClock->SyncTo(VideoClock);

      VideoTexture.Submit(frame.Frame->data[0], frame.Frame->data[1],
                          frame.Frame->data[2]);
      PlaybackStarted = true;
      av_frame_free(&frame.Frame);
      SDL_LockMutex(VideoStream->FrameLock);
      VideoStream->FrameQueue.pop();
      SDL_UnlockMutex(VideoStream->FrameLock);
      SDL_CondSignal(VideoStream->DecodeCond);
      break;
    }
  }
}

void VideoPlayer::Render(float videoAlpha) {
  if (IsPlaying && PlaybackStarted) {
    float widthScale = Profile::DesignWidth / VideoTexture.Width;
    float heightScale = Profile::DesignHeight / VideoTexture.Height;
    glm::vec4 tint = glm::vec4(1.0f);
    if (IsAlpha) tint.a = videoAlpha;
    Renderer->DrawVideoTexture(VideoTexture, glm::vec2(0.0f, 0.0f), tint,
                                 glm::vec2(widthScale, heightScale), 0.0f,
                                 IsAlpha);
  }
}

double VideoPlayer::GetTargetDelay(double duration) {
  double diff = VideoClock->Get() - MasterClock->Get();
  double sync_threshold = std::max(0.04, std::min(0.1, duration));
  if (!isnan(diff) && fabs(diff) < MaxFrameDuration) {
    if (diff <= -sync_threshold)
      duration = std::max(0.0, duration + diff);
    else if (diff >= sync_threshold && duration > 0.1)
      duration = duration + diff;
    else if (diff >= sync_threshold)
      duration = 2 * duration;
  }
  ImpLogSlow(LL_Trace, LC_Video, "Target delay: %f\n", duration);

  return duration;
}

void VideoPlayer::FillAudioBuffers() {
  while (FreeBufferCount && !AbortRequest) {
    int totalSize = 0;

    do {
      bool firstFrame = true;

      SDL_LockMutex(AudioStream->FrameLock);
      if (AudioStream->FrameQueue.empty()) {
        SDL_UnlockMutex(AudioStream->FrameLock);
        SDL_CondSignal(AudioStream->DecodeCond);
        continue;
      }
      auto aFrame = AudioStream->FrameQueue.front();
      SDL_UnlockMutex(AudioStream->FrameLock);

      if (aFrame.Serial == INT32_MIN) {
        SDL_LockMutex(AudioStream->FrameLock);
        AudioStream->FrameQueue.pop();
        SDL_UnlockMutex(AudioStream->FrameLock);
        break;
      }

      if (firstFrame) {
        BufferStartPositions[FirstFreeBuffer] =
            aFrame.Frame->best_effort_timestamp *
            av_q2d(AudioStream->stream->time_base) * aFrame.Frame->sample_rate;
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
      SDL_LockMutex(AudioStream->FrameLock);
      AudioStream->FrameQueue.pop();
      SDL_UnlockMutex(AudioStream->FrameLock);
      SDL_CondSignal(AudioStream->DecodeCond);
    } while (totalSize <= 4096);

    alSourceUnqueueBuffers(ALSource, 1, &BufferIds[FirstFreeBuffer]);
    FreeBufferCount--;

    alBufferData(BufferIds[FirstFreeBuffer], AL_FORMAT_STEREO16, HostBuffer,
                 totalSize, AudioStream->CodecContext->sample_rate);

    alSourceQueueBuffers(ALSource, 1, &BufferIds[FirstFreeBuffer]);

    FirstFreeBuffer++;
    FirstFreeBuffer %= AudioBufferCount;
  }
}

}  // namespace Video
}  // namespace Impacto
