#include "ffmpegplayer.h"

#include <avcpp/av.h>
#include <avcpp/avtime.h>
#include <avcpp/avutils.h>
#include <avcpp/codec.h>
#include <avcpp/formatcontext.h>
#include <avcpp/codeccontext.h>
#include <avcpp/timestamp.h>
#include <avcpp/packet.h>
#include <avcpp/rational.h>

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
#include "../profile/subtitle.h"
#ifndef IMPACTO_DISABLE_LIBASS
#include "../subtitle/ass/subtitlerenderer.h"
#endif

namespace Impacto {
namespace Video {

using namespace Impacto::Profile::ScriptVars;
Profile::Subtitle::SubtitleType SubtitleCodecToType(AVCodecID id) {
  switch (id) {
    case AV_CODEC_ID_TEXT:
    case AV_CODEC_ID_SRT:
    case AV_CODEC_ID_MOV_TEXT:
    case AV_CODEC_ID_SUBRIP:
      return +Profile::Subtitle::SubtitleType::Text;
    case AV_CODEC_ID_SSA:
    case AV_CODEC_ID_ASS:
      return +Profile::Subtitle::SubtitleType::Ass;
    case AV_CODEC_ID_HDMV_PGS_SUBTITLE:
      return +Profile::Subtitle::SubtitleType::Bitmap;
    default:
      return +Profile::Subtitle::SubtitleType::None;
  }
}

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
    case AudioBackendType::None:
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
                MediaType != AVMEDIA_TYPE_AUDIO &&
                MediaType != AVMEDIA_TYPE_SUBTITLE) {
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
  } else if constexpr (MediaType == AVMEDIA_TYPE_SUBTITLE) {
    rate = avStream.averageFrameRate().getDouble();
  }

  double duration = FormatContext.duration().seconds();
  double frameMultiplier = (rate);
  streamOpt.emplace(std::move(avStream), std::move(decoderContext),
                    (int)(frameMultiplier * duration));
};

template void FFmpegPlayer::OpenCodec(
    std::optional<FFmpegStream<AVMEDIA_TYPE_VIDEO>>& streamOpt,
    av::Stream&& avStream, int streamId);
template void FFmpegPlayer::OpenCodec(
    std::optional<FFmpegStream<AVMEDIA_TYPE_AUDIO>>& streamOpt,
    av::Stream&& avStream, int streamId);
template void FFmpegPlayer::OpenCodec(
    std::optional<FFmpegStream<AVMEDIA_TYPE_SUBTITLE>>& streamOpt,
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
  std::vector<std::pair<av::Stream, int>> embeddedSubStreams;
  for (size_t i = 0; i < FormatContext.streamsCount(); ++i) {
    auto st = FormatContext.stream(i);
    if (st.isVideo()) {
      videoStreamId = (int)i;
      videoStream = st;
    } else if (st.isAudio() && !NoAudio) {
      audioStreamId = (int)i;
      audioStream = st;
    } else if (st.isSubtitle()) {
      embeddedSubStreams.emplace_back(st, (int)i);
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
  using namespace std::literals::chrono_literals;
  MaxFrameDuration = {FormatContext.inputFormat().flags() & AVFMT_TS_DISCONT
                          ? Clock::Microseconds(10s)
                          : Clock::Microseconds(3600s)};

  // Danger zone
  ReadThread = std::thread{&FFmpegPlayer::Read, this};
  if (VideoStream) {
    VideoStream->DecoderThread =
        std::thread{&FFmpegPlayer::Decode<AVMEDIA_TYPE_VIDEO>, this,
                    std::ref(*VideoStream)};
  }
  if (AudioStream && !NoAudio) {
    MasterClock = &AudioPlayer->GetClock();
    AudioStream->DecoderThread =
        std::thread{&FFmpegPlayer::Decode<AVMEDIA_TYPE_AUDIO>, this,
                    std::ref(*AudioStream)};
  }
  if (Profile::GameFeatures & GameFeature::Subtitles) {
    InitSubtitles(embeddedSubStreams);
  }

  IsPlaying = true;
}

void FFmpegPlayer::InitSubtitles(
    std::vector<std::pair<av::Stream, int>>& embeddedSubStreams) {
  using Profile::Subtitle::SubtitleMappings;
  using Profile::Subtitle::SubtitleType;
  using namespace Subtitle;
  auto initSubPlayer = [this] {
    if (!SubPlayer) {
      if (VideoTexture)
        SubPlayer.emplace(VideoTexture->Width, VideoTexture->Height);
      else
        SubPlayer.emplace(Profile::DesignWidth, Profile::DesignHeight);
    }
  };
  auto mappingsItr = SubtitleMappings.find(StreamPtr->Meta.FileName);

  // Tracks embedded in video
  for (auto& [subtitleStream, id] : embeddedSubStreams) {
    assert(subtitleStream.isSubtitle() && subtitleStream.isValid());

    std::optional<Impacto::Video::FFmpegStream<AVMEDIA_TYPE_SUBTITLE>>
        streamOpt;
    OpenCodec<AVMEDIA_TYPE_SUBTITLE>(streamOpt, std::move(subtitleStream), id);
    if (!streamOpt) continue;

    initSubPlayer();
    auto& subStream =
        EmbeddedSubtitleStreams.emplace_back(std::move(*streamOpt));
    const auto subtitleType =
        SubtitleCodecToType(subStream.CodecContext.codec().id());
    subStream.DecoderThread =
        std::thread{&FFmpegPlayer::Decode<AVMEDIA_TYPE_SUBTITLE>, this,
                    std::ref(subStream)};

    // Optionally tag embedded subtitle tracks in lua
    Profile::SubtitleConfigType subConfig = +Profile::SubtitleConfigType::All;
    if (mappingsItr != SubtitleMappings.end()) {
      const auto subFileItr = std::find_if(
          mappingsItr->second.begin(), mappingsItr->second.end(),
          [&subStream](const auto& subFile) {
            return subFile.Id && subStream.AvStream.id() == *subFile.Id;
          });
      if (subFileItr != mappingsItr->second.end())
        subConfig = subFileItr->Config;
    }
#ifndef IMPACTO_DISABLE_LIBASS
    if (subtitleType == +SubtitleType::Ass) {
      std::string_view subHeader = subStream.CodecContext.subtitleHeader();
      SubPlayer->AddTrack<Ass::SubtitleRenderTrack>(subStream.AvStream.id(),
                                                    subConfig, subHeader);
    }
#endif
  }

  // External subs mapped through lua
  if (mappingsItr != SubtitleMappings.end()) {
    initSubPlayer();
    int trackId = 0;
    for (auto const& subFile : mappingsItr->second) {
      if (!subFile.Path) continue;
      SubPlayer->AddTrackFile(trackId++, subFile.Type, *subFile.Path,
                              subFile.Config);
    }
  }
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

  FrameTimer = Clock::MonotonicTime{};
  PreviousFrameTimestamp = std::nullopt;
  SeekRequest = false;
  ReaderEOF = false;
}

void FFmpegPlayer::Read() {
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
      if (AudioStream &&
          item.Packet.streamIndex() == AudioStream->AvStream.index()) {
        item.Serial = AudioStream->PacketQueueSerial;
        while (!AudioStream->PacketQueue.wait_enqueue_timed(std::move(item),
                                                            300)) {
          if (AbortRequest) break;
        };
      } else if (item.Packet.streamIndex() == VideoStream->AvStream.index()) {
        item.Serial = VideoStream->PacketQueueSerial;
        while (!VideoStream->PacketQueue.wait_enqueue_timed(std::move(item),
                                                            300)) {
          if (AbortRequest) break;
        };
      } else {
        for (auto& subtitleStream : EmbeddedSubtitleStreams) {
          if (item.Packet.streamIndex() != subtitleStream.AvStream.index())
            continue;
          item.Serial = subtitleStream.PacketQueueSerial;
          while (!subtitleStream.PacketQueue.wait_enqueue_timed(std::move(item),
                                                                300)) {
            if (AbortRequest) break;
          };
        }
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
      for (auto& subtitleStream : EmbeddedSubtitleStreams) {
        while (!subtitleStream.PacketQueue.wait_enqueue_timed(std::move(item),
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
void FFmpegPlayer::Decode(FFmpegStream<MediaType>& stream) {
  auto verifyPacket =
      [this, &stream](Impacto::Video::AVPacketItem const* peekedPacket) {
        while (true) {
          int prevSerial = stream.CurrentPacketSerial;
          stream.CurrentPacketSerial = peekedPacket->Serial;
          if (stream.CurrentPacketSerial == INT32_MIN) {
            break;
          }
          if (prevSerial != stream.CurrentPacketSerial) {
            avcodec_flush_buffers(VideoStream->CodecContext.raw());
            if constexpr (MediaType == AVMEDIA_TYPE_AUDIO) {
              AudioPlayer->Stop();
            }
          }
          if (stream.PacketQueueSerial == stream.CurrentPacketSerial) {
            break;
          }
          if (AbortRequest) return AVPacketItem{};
        }
        AVPacketItem packet;
        auto& packetQueue = stream.PacketQueue;
        while (!packetQueue.wait_dequeue_timed(packet, 300)) {
          if (AbortRequest) return AVPacketItem{};
        }
        return packet;
      };

  auto pushFrame = [&stream, this](Frame_t<MediaType>&& frame) {
    AVDecodedItem<MediaType> item;
    item.Frame = std::move(frame);
    if (!item.Frame) {
      item.Serial = INT32_MIN;
    } else {
      item.Serial = stream.PacketQueueSerial;
      item.Timestamp = item.Frame.pts();
    }
    auto& frameQueue = stream.FrameQueue;

    while (!frameQueue.wait_enqueue_timed(std::move(item), 300)) {
      if (AbortRequest) return;
    }
  };

  while (!AbortRequest) {
    AVPacketItem const* peek = stream.PacketQueue.peek();
    if (peek == nullptr) continue;

    AVPacketItem packet = verifyPacket(peek);
    std::error_code ec;

    if (packet.Serial != INT32_MIN) {
      Frame_t<MediaType> frame = stream.CodecContext.decode(packet.Packet, ec);
      if (ec) {
        ImpLog(LogLevel::Error, LogChannel::Video, "Failed to decode {:s}",
               ec.message());
      }
      if (!frame) continue;  // Skip Empty Padding Frames
      if constexpr (MediaType == AVMEDIA_TYPE_SUBTITLE) {
        auto* rawSubtitle = frame.raw();
        if (rawSubtitle->start_display_time == 0 &&
            rawSubtitle->end_display_time == 0) {
          rawSubtitle->end_display_time = packet.Packet.duration();
        }
      }
      pushFrame(std::move(frame));
    } else {
      // Flush decoder since packets are finished
      bool decode = true;
      while (decode && !AbortRequest) {
        if (ec) {
          ImpLog(LogLevel::Error, LogChannel::Video, "Failed to decode {:s}",
                 ec.message());
        }
        Frame_t<MediaType> frame = stream.CodecContext.decode({}, ec);
        decode = frame;
        pushFrame(std::move(frame));
      }
    }
  }
}

template void FFmpegPlayer::Decode<AVMEDIA_TYPE_VIDEO>(
    FFmpegStream<AVMEDIA_TYPE_VIDEO>& stream);
template void FFmpegPlayer::Decode<AVMEDIA_TYPE_AUDIO>(
    FFmpegStream<AVMEDIA_TYPE_AUDIO>& stream);
template void FFmpegPlayer::Decode<AVMEDIA_TYPE_SUBTITLE>(
    FFmpegStream<AVMEDIA_TYPE_SUBTITLE>& stream);

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
    for (auto& subStream : EmbeddedSubtitleStreams) {
      subStream.DecoderThread.join();
    }
    EmbeddedSubtitleStreams.clear();
    SubPlayer.reset();
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
    using namespace std::literals::chrono_literals;
    if (AudioStream) AudioPlayer->Process();

    if (Profile::GameFeatures & GameFeature::Subtitles) {
      if (SubPlayer) SubPlayer->Update(MasterClock->Get());
      UpdateSubtitles();
    }

    Clock::Microseconds duration{};
    Clock::MonotonicTime time;

    AVDecodedItem<AVMEDIA_TYPE_VIDEO>* const frame =
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
    time = Clock::Now();
    if (FrameTimer == Clock::MonotonicTime{}) {
      FrameTimer = time;
    }
    if (PreviousFrameTimestamp) {
      auto inverseFrameRate = av::Rational(1, 30);
      size_t frameNum = av_rescale_q(frame->Timestamp.timestamp(),
                                     frame->Timestamp.timebase().getValue(),
                                     inverseFrameRate.getValue());
      // This isn't the place for it but I can't think of
      // anything right now
      ScrWork[SW_MOVIEFRAME] = (int)frameNum;
      duration = (frame->Timestamp.toDuration<Clock::Microseconds>() -
                  PreviousFrameTimestamp->toDuration<Clock::Microseconds>());
    }

    if (AudioStream) {
      duration = GetTargetDelay(Clock::Microseconds(duration));
    } else {
      auto fps = VideoStream->AvStream.averageFrameRate();
      duration = std::chrono::duration_cast<Clock::Microseconds>(
                     std::chrono::seconds(fps.getDenominator())) /
                 fps.getNumerator();
    }

    if (time < FrameTimer + duration) {
      return;
    }
    FrameTimer += duration_cast<Clock::MonotonicTime::duration>(duration);
    if (duration > 0s && (time - FrameTimer) > 0.1s) {
      FrameTimer = time;
    }

    PreviousFrameTimestamp = frame->Timestamp;

    VideoTexture->Submit(frame->Frame.data(0), frame->Frame.data(1),
                         frame->Frame.data(2));
    VideoClock.Set(frame->Timestamp.toDuration<Clock::Microseconds>(),
                   frame->Serial);
    MasterClock->SyncTo(&VideoClock);
    AVDecodedItem<AVMEDIA_TYPE_VIDEO> unusedFrame;
    VideoStream->FrameQueue.wait_dequeue(unusedFrame);
    PlaybackStarted = true;
  }
}

void FFmpegPlayer::UpdateSubtitles() {
  for (auto& subtitleStream : EmbeddedSubtitleStreams) {
    Video::AVDecodedItem<AVMEDIA_TYPE_SUBTITLE> subtitle;
    const bool hasSubtitle = subtitleStream.FrameQueue.try_dequeue(subtitle);
    if (!hasSubtitle) continue;

    auto const& rawSubtitleData = subtitle.Frame.raw();
    for (unsigned rectI = 0; rectI < rawSubtitleData->num_rects; rectI++) {
      auto const& rect = rawSubtitleData->rects[rectI];
      Subtitle::SubtitleEntry subEntry;
      switch (rect->type) {
        case SUBTITLE_ASS:
        case SUBTITLE_TEXT:
          subEntry.Data = std::string(rect->ass);
          break;
        case SUBTITLE_BITMAP: {
          Subtitle::SubtitleEntry::BitmapData bmp{
              .X = rect->x,
              .Y = rect->y,
              .W = rect->w,
              .H = rect->h,
              .NbColors = rect->nb_colors,
          };
          for (uint8_t bmpI = 0; bmpI < bmp.Data.size(); ++bmpI) {
            bmp.LineSize[bmpI] = rect->linesize[bmpI];
            bmp.Data[bmpI] = std::make_unique<uint8_t[]>(rect->linesize[bmpI]);
            std::copy(rect->data[bmpI], rect->data[bmpI] + rect->linesize[bmpI],
                      bmp.Data[bmpI].get());
          }
        } break;
        case SUBTITLE_NONE:
        default:
          break;
      }
      subEntry.Flags = rect->flags;
      subEntry.StartMs =
          subtitle.Timestamp.toDuration<std::chrono::milliseconds>() +
          std::chrono::milliseconds(rawSubtitleData->start_display_time);
      subEntry.Duration =
          std::chrono::milliseconds(rawSubtitleData->end_display_time -
                                    rawSubtitleData->start_display_time);
      SubPlayer->PushEntry(subtitleStream.AvStream.id(), std::move(subEntry));
    }
  }
}

void FFmpegPlayer::Render(float videoAlpha) {
  if (!IsPlaying || !PlaybackStarted) return;

  const RectF dest = {0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight};
  const glm::vec4 tint = {1.0f, 1.0f, 1.0f, videoAlpha};
  Renderer->DrawVideoTexture(*VideoTexture, dest, tint, IsAlpha);
  if (SubPlayer) SubPlayer->Render();
}

Clock::Microseconds FFmpegPlayer::GetTargetDelay(Clock::Microseconds duration) {
  using namespace std::literals::chrono_literals;
  using namespace std::chrono;
  using Us = Clock::Microseconds;
  const Us videoClockTime = VideoClock.Get();
  const Us masterClockTime = MasterClock->Get();
  const Us diff = videoClockTime - masterClockTime;
  const Us sync_threshold = std::clamp<Us>(duration, 40ms, 100ms);
  if ((videoClockTime != Us{} && masterClockTime != Us{}) &&
      abs(diff) < MaxFrameDuration) {
    if (diff <= -sync_threshold)
      duration = std::max<Us>(0s, duration + diff);
    else if (diff >= sync_threshold && duration > 100ms)
      duration = duration + diff;
    else if (diff >= sync_threshold)
      duration = 2 * duration;
  }
  ImpLogSlow(LogLevel::Trace, LogChannel::Video, "Target delay: {:f}\n",
             duration_cast<std::chrono::duration<double>>(duration).count());

  return duration;
}
}  // namespace Video
}  // namespace Impacto
