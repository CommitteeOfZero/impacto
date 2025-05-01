#include "ffmpegplayer.h"

#include <av.h>
#include <avtime.h>
#include <avutils.h>
#include <codec.h>
#include <dictionary.h>
#include <formatcontext.h>
#include <codeccontext.h>
#include <timestamp.h>
#include <packet.h>
#include <rational.h>

extern "C" {
#include <libavutil/avutil.h>
#include <libavutil/time.h>
}

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <optional>
#include <system_error>
#include <utility>
#include "ffmpegstream.h"

#include "../log.h"
#include "../profile/game.h"
#include "../io/stream.h"
#include "../audio/ffmpegaudioplayer.h"
#ifndef IMPACTO_DISABLE_OPENAL
#include "../audio/openal/ffmpegaudioplayer.h"
#endif
#include "../profile/scriptvars.h"

namespace Impacto {
namespace Video {

using namespace Impacto::Profile::ScriptVars;

int FFmpegFileIO::read(uint8_t* data, size_t size) {
  if (!FileStream) return -1;
  uint64_t bytesRead = FileStream->Read(data, size);
  if ((bytesRead == static_cast<uint64_t>(IoError_Fail) ||
       bytesRead == static_cast<uint64_t>(IoError_Eof)))
    return AVERROR_EOF;

  return (int)bytesRead;
}

int64_t FFmpegFileIO::seek(int64_t offset, int whence) {
  if (!FileStream) return -1;
  if (whence == AVSEEK_SIZE) return FileStream->Meta.Size;
  int64_t newPos = FileStream->Seek(offset, whence);
  if (newPos == IoError_Fail) return -1;
  return newPos;
}

FFmpegPlayer::~FFmpegPlayer() { IsInit = false; }

void FFmpegPlayer::Init() {
  assert(IsInit == false);

  switch (Profile::ActiveAudioBackend) {
#ifndef IMPACTO_DISABLE_OPENAL
    case AudioBackendType::OpenAL: {
      AudioPlayer.reset(new Audio::OpenAL::FFmpegAudioPlayer(this));
    } break;
#endif
    default: {
      AudioPlayer.reset(new Audio::FFmpegAudioPlayer(this));
      NoAudio = true;
      ImpLog(
          LogLevel::Warning, LogChannel::Video,
          "No audio backend available, you will not hear audio in videos.\n");
    } break;
  }
  av::init();
  // av::set_logging_level("debug");
  AudioPlayer->Init();

  IsInit = true;
}

template <AVMediaType MediaType>
void FFmpegPlayer::OpenCodec(std::optional<FFmpegStream<MediaType>>& streamOpt,
                             av::Stream&& avStream, int streamId) {
  if constexpr (MediaType != AVMEDIA_TYPE_VIDEO &&
                MediaType != AVMEDIA_TYPE_AUDIO) {
    static_assert(MediaType && false, "Unsupported MediaType");
  }

  const auto codec =
      av::findDecodingCodec(avStream.codecParameters().codecId());
  if (!codec.canDecode()) {
    ImpLog(LogLevel::Error, LogChannel::Video, "Unsupported codec!\n");
    streamOpt.reset();
  }

  std::error_code ec;

  DecodingContext_t<MediaType> decoderContext{avStream, codec};
  decoderContext.open({{{"threads", "auto"}}}, ec);
  decoderContext.setRefCountedFrames(true);
  if (ec) {
    ImpLog(LogLevel::Error, LogChannel::Audio,
           "Failed to open codec, error: {:s}", ec.message());
  }

  double rate = 1;
  if constexpr (MediaType == AVMEDIA_TYPE_VIDEO) {
    rate = avStream.averageFrameRate().getDouble();
  } else if constexpr (MediaType == AVMEDIA_TYPE_AUDIO) {
    rate = decoderContext.sampleRate();
    decoderContext.setChannelLayout(AV_CH_LAYOUT_STEREO);
  }
  double duration = avStream.duration().seconds();
  double frameMultiplier = (rate);
  streamOpt.emplace(std::move(avStream), std::move(decoderContext),
                    frameMultiplier * duration);
};

template void FFmpegPlayer::OpenCodec(
    std::optional<FFmpegStream<AVMEDIA_TYPE_VIDEO>>& streamOpt,
    av::Stream&& avStream, int streamId);
template void FFmpegPlayer::OpenCodec(
    std::optional<FFmpegStream<AVMEDIA_TYPE_AUDIO>>& streamOpt,
    av::Stream&& avStream, int streamId);

void FFmpegPlayer::Play(Io::Stream* stream, bool looping, bool alpha) {
  // Don't do anything if we don't have the video system
  if (!IsInit) return;
  if (stream == nullptr) {
    ImpLog(LogLevel::Error, LogChannel::Video,
           "Stream was a nullptr! This means the caller is buggy. Backing "
           "out.\n");
    return;
  }
  StreamPtr.reset(stream);
  AbortRequest = false;
  SeekRequest = false;
  Looping = looping;
  IsAlpha = alpha;
  ImpLog(LogLevel::Info, LogChannel::Video, "Opening file: {:s} from: {:s}\n",
         stream->Meta.FileName, stream->Meta.ArchiveFileName);

  std::error_code ec;
  IoContext = FFmpegFileIO{stream};
  FormatContext.openInput(&IoContext, ec, FILESTREAMBUFFERSZ);
  if (ec) {
    ImpLog(LogLevel::Error, LogChannel::Video,
           "Error opening file, error: {:s}\n", ec.message());
    StreamPtr.reset();
    return;
  }

  FormatContext.findStreamInfo(ec);
  if (ec) {
    ImpLog(LogLevel::Error, LogChannel::Video,
           "Error opening file, error: {:s}\n", ec.message());
    StreamPtr.reset();
    return;
  }

  av::Stream videoStream;
  int videoStreamId = AVERROR_STREAM_NOT_FOUND;
  av::Stream audioStream;
  int audioStreamId = AVERROR_STREAM_NOT_FOUND;
  for (size_t i = 0; i < FormatContext.streamsCount(); ++i) {
    auto st = FormatContext.stream(i);
    if (st.isVideo()) {
      videoStreamId = i;
      videoStream = st;
    } else if (st.isAudio()) {
      audioStreamId = i;
      audioStream = st;
    } else {
      st.raw()->discard = AVDISCARD_ALL;
    }
  }

  if (videoStream.isVideo() && videoStream.isValid()) {
    OpenCodec<AVMEDIA_TYPE_VIDEO>(VideoStream, std::move(videoStream),
                                  videoStreamId);
    ScrWork[SW_MOVIEFRAME] = 0;
    ScrWork[SW_MOVIETOTALFRAME] = VideoStream->Duration;
    if (!VideoTexture)
      VideoTexture =
          Renderer->CreateYUVFrame((float)VideoStream->CodecContext.width(),
                                   (float)VideoStream->CodecContext.height());
    else {
      VideoTexture->Width = (float)VideoStream->CodecContext.width();
      VideoTexture->Height = (float)VideoStream->CodecContext.height();
    }
  }
  if (audioStream.isAudio() && audioStream.isValid()) {
    OpenCodec<AVMEDIA_TYPE_AUDIO>(AudioStream, std::move(audioStream),
                                  audioStreamId);
    AudioPlayer->InitConvertContext(AudioStream->CodecContext.raw());
  }

  VideoClock = Clock();
  MasterClock = &VideoClock;
  MaxFrameDuration = {
      FormatContext.inputFormat().flags() & AVFMT_TS_DISCONT ? 10.0 : 3600.0};

  // Danger zone
  ReadThread = std::thread{&FFmpegPlayer::Read, this};
  if (VideoStream) {
    VideoStream->DecoderThread =
        std::thread{&FFmpegPlayer::Decode<AVMEDIA_TYPE_VIDEO>, this};
  }
  if (AudioStream && !NoAudio) {
    MasterClock = &AudioPlayer->GetClock();
    AudioStream->DecoderThread =
        std::thread{&FFmpegPlayer::Decode<AVMEDIA_TYPE_AUDIO>, this};
  }

  IsPlaying = true;
}

void FFmpegPlayer::HandleSeekRequest() {
  std::error_code ec;
  FormatContext.seek(av::Timestamp(SeekPosition, av::TimeBaseQ), ec);
  if (ec) {
    ImpLog(LogLevel::Error, LogChannel::Video,
           "Error encountered while seeking, error: {:s}", ec.message());
  }

  if (VideoStream) {
    VideoStream->FlushPacketQueue();
    VideoStream->FlushFrameQueue();
  }

  if (AudioStream) {
    AudioStream->FlushPacketQueue();
    AudioStream->FlushFrameQueue();
  }

  FrameTimer = 0;
  PreviousFrameTimestamp = -1;
  SeekRequest = false;
  ReaderEOF = false;
}

void FFmpegPlayer::Read() {
  std::mutex waitMutex;
  while (!AbortRequest) {
    if (SeekRequest) {
      HandleSeekRequest();
    }

    AVPacketItem item;
    if (ReaderEOF) {
      continue;
    }
    std::error_code ec;
    item.Packet = FormatContext.readPacket(ec);
    if (ec) {
      ImpLog(LogLevel::Error, LogChannel::Video, "Uh oh {:s}\n", ec.message());
    }
    if (item.Packet) {
      if (item.Packet.streamIndex() == VideoStream->stream.index()) {
        item.Serial = VideoStream->PacketQueueSerial;
        while (!VideoStream->PacketQueue.wait_enqueue_timed(std::move(item),
                                                            300)) {
          if (AbortRequest) break;
        };
      } else if (AudioStream &&
                 item.Packet.streamIndex() == AudioStream->stream.index()) {
        item.Serial = AudioStream->PacketQueueSerial;
        while (!AudioStream->PacketQueue.wait_enqueue_timed(std::move(item),
                                                            300)) {
          if (AbortRequest) break;
        };
      }
    } else {
      ImpLog(LogLevel::Debug, LogChannel::Video, "EOF!\n");
      ReaderEOF = true;
      item.Serial = INT32_MIN;
      if (AudioStream) {
        while (!AudioStream->PacketQueue.wait_enqueue_timed(std::move(item),
                                                            300)) {
          if (AbortRequest) break;
        };
      }
      while (
          !VideoStream->PacketQueue.wait_enqueue_timed(std::move(item), 300)) {
        if (AbortRequest) break;
      };
    }
  }
}

template <AVMediaType MediaType>
void FFmpegPlayer::Decode() {
  std::mutex waitMutex;
  FFmpegStream<MediaType>* stream;
  if constexpr (MediaType == AVMEDIA_TYPE_VIDEO)
    stream = std::addressof(*VideoStream);
  else if constexpr (MediaType == AVMEDIA_TYPE_AUDIO)
    stream = std::addressof(*AudioStream);

  auto verifyPacket =
      [this, stream](Impacto::Video::AVPacketItem const* peekedPacket) {
        while (true) {
          int prevSerial = stream->CurrentPacketSerial;
          stream->CurrentPacketSerial = peekedPacket->Serial;
          if (stream->CurrentPacketSerial == INT32_MIN) {
            break;
          }
          if (prevSerial != stream->CurrentPacketSerial) {
            avcodec_flush_buffers(VideoStream->CodecContext.raw());
            if constexpr (MediaType == AVMEDIA_TYPE_AUDIO) {
              AudioPlayer->Stop();
            }
          }
          if (stream->PacketQueueSerial == stream->CurrentPacketSerial) {
            break;
          }
        }
        AVPacketItem packet;
        auto& packetQueue = stream->PacketQueue;
        while (!packetQueue.wait_dequeue_timed(packet, 300)) {
          if (AbortRequest) return AVPacketItem{};
        }
        return packet;
      };

  auto pushFrame = [stream, this](Frame_t<MediaType>&& frame) {
    AVFrameItem<MediaType> item;
    item.Frame = std::move(frame);
    if (!item.Frame) {
      item.Serial = INT32_MIN;
    } else {
      item.Serial = stream->PacketQueueSerial;
      item.Timestamp = item.Frame.pts();
    }
    auto& frameQueue = stream->FrameQueue;

    while (!frameQueue.wait_enqueue_timed(std::move(item), 300)) {
      if (AbortRequest) return;
    }
  };

  while (!AbortRequest) {
    AVPacketItem const* peek = stream->PacketQueue.peek();
    if (peek == nullptr) continue;

    AVPacketItem packet = verifyPacket(peek);
    std::error_code ec;

    if (packet.Serial != INT32_MIN) {
      Frame_t<MediaType> frame = stream->CodecContext.decode(packet.Packet, ec);
      if (ec) {
        ImpLog(LogLevel::Error, LogChannel::Video, "Failed to decode {:s}",
               ec.message());
      }
      if (!frame) continue;  // Skip Empty Padding Frames
      pushFrame(std::move(frame));
    } else {
      // Flush decoder since packets are finished
      bool decode = true;
      while (decode) {
        if (ec) {
          ImpLog(LogLevel::Error, LogChannel::Video, "Failed to decode {:s}",
                 ec.message());
        }
        Frame_t<MediaType> frame = stream->CodecContext.decode({}, ec);
        decode = frame;
        pushFrame(std::move(frame));
      }
    }
  }
}

template void FFmpegPlayer::Decode<AVMEDIA_TYPE_VIDEO>();
template void FFmpegPlayer::Decode<AVMEDIA_TYPE_AUDIO>();

void FFmpegPlayer::Stop() {
  if (IsPlaying) {
    IsPlaying = false;
    PlaybackStarted = false;
    AbortRequest = true;
    ReadThread.join();
    ReaderEOF = false;
    if (AudioStream) {
      AudioStream->DecoderThread.join();
      AudioStream.reset();
      AudioPlayer->Unload();
    }
    if (VideoStream) {
      VideoStream->DecoderThread.join();
      VideoStream.reset();
    }
    FrameTimer = {};
    PreviousFrameTimestamp = {};
    FormatContext.close();
    StreamPtr.reset();
    if (VideoTexture) {
      VideoTexture->Release();
      VideoTexture = 0;
    }
    SetFlag(SF_MOVIEPLAY, false);
  }
}

void FFmpegPlayer::Seek(int64_t pos) {
  SeekRequest = true;
  SeekPosition = pos;
  ReadCond.notify_one();
}

void FFmpegPlayer::Update(float dt) {
  if (IsPlaying) {
    if (AudioStream) AudioPlayer->Process();
    double duration{};
    double time;

    AVFrameItem<AVMEDIA_TYPE_VIDEO>* const frame =
        VideoStream->FrameQueue.peek();
    if (frame == nullptr) return;
    SetFlag(SF_MOVIE_DRAWWAIT, false);

    if (frame->Serial == INT32_MIN) {
      if (Looping) {
        Seek(0);
      } else {
        Stop();
      }
      return;
    }
    time = av_gettime_relative() / 1000000.0;
    if (!FrameTimer) {
      FrameTimer = time;
    }
    if (PreviousFrameTimestamp != -1) {
      auto inverseFrameRate = av::Rational(1, 30);
      size_t frameNum = av_rescale_q(frame->Timestamp.timestamp(),
                                     frame->Timestamp.timebase().getValue(),
                                     inverseFrameRate.getValue());
      // This isn't the place for it but I can't think of
      // anything right now
      ScrWork[SW_MOVIEFRAME] = (int)frameNum;
      duration = (frame->Timestamp.seconds() - PreviousFrameTimestamp);
    }

    if (AudioStream) {
      duration = GetTargetDelay(duration);
    } else {
      duration =
          ((double)VideoStream->stream.averageFrameRate().getDenominator() /
           VideoStream->stream.averageFrameRate().getNumerator());
    }

    if (time < FrameTimer + duration) {
      return;
    }

    FrameTimer += duration;
    if (duration > 0 && (time - FrameTimer) > 0.1) {
      FrameTimer = time;
    }

    PreviousFrameTimestamp = frame->Timestamp;

    VideoTexture->Submit(frame->Frame.data(0), frame->Frame.data(1),
                         frame->Frame.data(2));
    VideoClock.Set(frame->Timestamp.seconds(), frame->Serial);
    MasterClock->SyncTo(&VideoClock);
    AVFrameItem<AVMEDIA_TYPE_VIDEO> unused;
    VideoStream->FrameQueue.wait_dequeue(unused);
    PlaybackStarted = true;
  }
}

void FFmpegPlayer::Render(float videoAlpha) {
  if (IsPlaying && PlaybackStarted) {
    const RectF dest = {0.0f, 0.0f, Profile::DesignWidth,
                        Profile::DesignHeight};
    const glm::vec4 tint = {1.0f, 1.0f, 1.0f, videoAlpha};
    Renderer->DrawVideoTexture(*VideoTexture, dest, tint, IsAlpha);
  }
}

double FFmpegPlayer::GetTargetDelay(double duration) {
  double diff = VideoClock.Get() - MasterClock->Get();
  double sync_threshold = std::max(0.04, std::min(0.1, duration));
  if (!isnan(diff) && fabs(diff) < MaxFrameDuration) {
    if (diff <= -sync_threshold)
      duration = std::max(0.0, duration + diff);
    else if (diff >= sync_threshold && duration > 0.1)
      duration = duration + diff;
    else if (diff >= sync_threshold)
      duration = 2 * duration;
  }
  ImpLogSlow(LogLevel::Trace, LogChannel::Video, "Target delay: {:f}\n",
             duration);

  return duration;
}
}  // namespace Video
}  // namespace Impacto
