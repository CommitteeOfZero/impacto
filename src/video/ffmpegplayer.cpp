#include "ffmpegplayer.h"
#include "ffmpegstream.h"

#include "../log.h"
#include "../profile/game.h"
#include "../io/inputstream.h"
#include "../audio/ffmpegaudioplayer.h"
#ifndef IMPACTO_DISABLE_OPENAL
#include "../audio/openal/ffmpegaudioplayer.h"
#endif

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavformat/avformat.h>
};

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
  ((FFmpegPlayer*)ptr)->Read();
  return 0;
}

static int PlayerDecodeVideo(void* ptr) {
  ((FFmpegPlayer*)ptr)->Decode(AVMEDIA_TYPE_VIDEO);
  return 0;
}

static int PlayerDecodeAudio(void* ptr) {
  ((FFmpegPlayer*)ptr)->Decode(AVMEDIA_TYPE_AUDIO);
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

FFmpegPlayer::~FFmpegPlayer() { IsInit = false; }

void FFmpegPlayer::Init() {
  assert(IsInit == false);

  switch (Profile::ActiveAudioBackend) {
#ifndef IMPACTO_DISABLE_OPENAL
    case AudioBackendType::OpenAL: {
      AudioPlayer = new Audio::OpenAL::FFmpegAudioPlayer(this);
    } break;
#endif
    default: {
      AudioPlayer = new Audio::FFmpegAudioPlayer(this);
      NoAudio = true;
      ImpLog(
          LL_Warning, LC_Video,
          "No audio backend available, you will not hear audio in videos.\n");
    } break;
  }
  AudioPlayer->Init();

  ReadCond = SDL_CreateCond();

  IsInit = true;
}

void FFmpegPlayer::Play(Io::InputStream* stream, bool looping, bool alpha) {
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

  // Find and open the video codec
  if (videoStreamId != AVERROR_STREAM_NOT_FOUND) {
    const auto codec = avcodec_find_decoder(videoStream->codecpar->codec_id);
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
    if (!VideoTexture)
      VideoTexture =
          Renderer->CreateYUVFrame(codecCtx->width, codecCtx->height);
    else {
      VideoTexture->Width = codecCtx->width;
      VideoTexture->Height = codecCtx->height;
    }
  }

  // Find and open the audio codec
  if (audioStreamId != AVERROR_STREAM_NOT_FOUND) {
    const auto codec = avcodec_find_decoder(audioStream->codecpar->codec_id);
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

    AudioPlayer->InitConvertContext(codecCtx);
  }

  VideoClock = new Clock();
  ExternalClock = new Clock();

  MasterClock = ExternalClock;

  MaxFrameDuration =
      (FormatContext->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0;

  // Danger zone
  ReadThreadID = SDL_CreateThread(&PlayerRead, "videoplayer::read", this);
  if (VideoStream) {
    VideoStream->DecoderThreadID =
        SDL_CreateThread(&PlayerDecodeVideo, "videoplayer::decodevideo", this);
  }
  if (AudioStream && !NoAudio) {
    MasterClock = AudioPlayer->GetClock();
    AudioStream->DecoderThreadID =
        SDL_CreateThread(&PlayerDecodeAudio, "videoplayer::decodeaudio", this);
  }

  IsPlaying = true;
}

bool FFmpegPlayer::QueuesHaveEnoughPackets() {
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

void FFmpegPlayer::Read() {
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
      ReaderEOF = false;

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

void FFmpegPlayer::Decode(AVMediaType avType) {
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
      duration = (frameRate.num && frameRate.den
                      ? av_q2d({frameRate.den, frameRate.num})
                      : 0.0);
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
          AudioPlayer->Stop();
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
        if (frame->best_effort_timestamp == AV_NOPTS_VALUE) {
          item.Timestamp = NAN;
        } else {
          item.Timestamp =
              frame->best_effort_timestamp * av_q2d(stream->stream->time_base);
        }
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

void FFmpegPlayer::Stop() {
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
    AudioPlayer->Unload();
    FrameTimer = 0.0;
    PreviousFrameTimestamp = 0.0;
    avformat_close_input(&FormatContext);
    if (IoContext)
      reinterpret_cast<Io::InputStream*>(IoContext->opaque)->~InputStream();
    avio_context_free(&IoContext);
    if (VideoTexture) {
      VideoTexture->Release();
      VideoTexture = 0;
    }
  }
}

void FFmpegPlayer::Seek(int64_t pos) {
  SeekRequest = true;
  SeekPosition = pos;
  SDL_CondSignal(ReadCond);
}

void FFmpegPlayer::Update(float dt) {
  if (IsPlaying) {
    if (AudioStream) AudioPlayer->Process();
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
      } else if (isnan(frame.Timestamp)) {
        duration = frame.Duration;
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

      if (!isnan(frame.Timestamp)) {
        VideoClock->Set(frame.Timestamp, frame.Serial);
        ExternalClock->SyncTo(VideoClock);
      }

      VideoTexture->Submit(frame.Frame->data[0], frame.Frame->data[1],
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

void FFmpegPlayer::Render(float videoAlpha) {
  if (IsPlaying && PlaybackStarted) {
    float widthScale = Profile::DesignWidth / VideoTexture->Width;
    float heightScale = Profile::DesignHeight / VideoTexture->Height;
    glm::vec4 tint = glm::vec4(1.0f);
    if (IsAlpha) tint.a = videoAlpha;
    Renderer->DrawVideoTexture(VideoTexture, glm::vec2(0.0f, 0.0f), tint,
                               glm::vec2(widthScale, heightScale), 0.0f,
                               IsAlpha);
  }
}

double FFmpegPlayer::GetTargetDelay(double duration) {
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

}  // namespace Video
}  // namespace Impacto
