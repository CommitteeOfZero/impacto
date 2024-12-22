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
          LL_Warning, LC_Video,
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
    ImpLog(LL_Error, LC_Video, "Unsupported codec!\n");
    streamOpt.reset();
  }

  std::error_code ec;

  DecodingContext_t<MediaType> decoderContext{avStream, codec};
  decoderContext.open({{{"threads", "auto"}}}, ec);
  decoderContext.setRefCountedFrames(true);
  if (ec) {
    ImpLog(LL_Error, LC_Audio, "Failed to open codec, error: %s",
           ec.message().c_str());
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
    ImpLog(LL_Error, LC_Video,
           "Stream was a nullptr! This means the caller is buggy. Backing "
           "out.\n");
    return;
  }
  StreamPtr.reset(stream);
  AbortRequest = false;
  SeekRequest = false;
  Looping = looping;
  IsAlpha = alpha;
  ImpLog(LL_Info, LC_Video, "Opening file: %s from: %s\n",
         stream->Meta.FileName.c_str(), stream->Meta.ArchiveFileName.c_str());

  std::error_code ec;
  IoContext = FFmpegFileIO{stream};
  FormatContext.openInput(&IoContext, ec, FILESTREAMBUFFERSZ);
  if (ec) {
    ImpLog(LL_Error, LC_Video, "Error opening file, error: %s\n",
           ec.message().c_str());
    StreamPtr.reset();
    return;
  }

  FormatContext.findStreamInfo(ec);
  if (ec) {
    ImpLog(LL_Error, LC_Video, "Error opening file, error: %s\n",
           ec.message().c_str());
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
  ReadThreadID = std::thread{&FFmpegPlayer::Read, this};
  if (VideoStream) {
    VideoStream->DecoderThreadID =
        std::thread{&FFmpegPlayer::Decode<AVMEDIA_TYPE_VIDEO>, this};
  }
  if (AudioStream && !NoAudio) {
    MasterClock = &AudioPlayer->GetClock();
    AudioStream->DecoderThreadID =
        std::thread{&FFmpegPlayer::Decode<AVMEDIA_TYPE_AUDIO>, this};
  }

  IsPlaying = true;
}

bool FFmpegPlayer::QueuesHaveEnoughPackets() {
  const size_t videoQueueSize = [this]() {
    std::lock_guard videoPacketLock(VideoStream->PacketLock);
    return VideoStream->PacketQueue.size();
  }();

  const size_t audioQueueSize = [this]() -> size_t {
    if (AudioStream) {
      std::lock_guard audioPacketLock(AudioStream->PacketLock);
      return AudioStream->PacketQueue.size();
    }
    return 26;
  }();
  return (videoQueueSize > 25 && audioQueueSize > 25);
}

void FFmpegPlayer::HandleSeekRequest() {
  SeekRequest = false;
  ReaderEOF = false;
  std::error_code ec;
  FormatContext.seek(av::Timestamp(SeekPosition, av::TimeBaseQ), ec);
  if (ec) {
    ImpLog(LL_Error, LC_Video, "Error encountered while seeking, error: %s",
           ec.message().c_str());
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
}

void FFmpegPlayer::Read() {
  std::mutex waitMutex;
  while (!AbortRequest) {
    if (SeekRequest) {
      HandleSeekRequest();
    }

    AVPacketItem item;
    if (QueuesHaveEnoughPackets() || ReaderEOF) {
      std::unique_lock lock{waitMutex};
      ReadCond.wait_for(lock, std::chrono::seconds(3));
      continue;
    }
    std::error_code ec;
    item.Packet = FormatContext.readPacket(ec);
    if (ec) {
      ImpLog(LL_Error, LC_Video, "Uh oh %s\n", ec.message().c_str());
    }
    if (item.Packet) {
      if (item.Packet.streamIndex() == VideoStream->stream.index()) {
        item.Serial = VideoStream->PacketQueueSerial;

        std::lock_guard lock(VideoStream->PacketLock);
        VideoStream->PacketQueue.push(std::move(item));
      } else if (AudioStream &&
                 item.Packet.streamIndex() == AudioStream->stream.index()) {
        item.Serial = AudioStream->PacketQueueSerial;

        std::lock_guard lock(AudioStream->PacketLock);
        AudioStream->PacketQueue.push(std::move(item));
      }
    } else {
      ImpLog(LL_Debug, LC_Video, "EOF!\n");
      ReaderEOF = true;
      item.Serial = INT32_MIN;
      if (AudioStream) {
        std::lock_guard lock(AudioStream->PacketLock);
        AudioStream->PacketQueue.push(item);
      }

      std::lock_guard lock(VideoStream->PacketLock);
      VideoStream->PacketQueue.push(item);
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

  auto hasPacket = [this, stream, &waitMutex]() {
    std::unique_lock packetLock(stream->PacketLock);
    ImpLogSlow(LL_Trace, LC_Video, "%d->PacketQueue.size() = %d\n", MediaType,
               stream->PacketQueue.size());
    if (stream->PacketQueue.empty()) {
      packetLock.unlock();
      std::unique_lock lock(waitMutex);
      ReadCond.notify_one();
      ImpLogSlow(LL_Trace, LC_Video, "%d Decoder starving!\n", MediaType);
      stream->DecodeCond.wait(lock);
      return false;
    }
    return true;
  };

  auto verifyPacket = [this, stream]() {
    AVPacketItem packet;
    while (true) {
      int prevSerial = stream->CurrentPacketSerial;
      std::unique_lock packetLock(stream->PacketLock);
      packet = std::move(stream->PacketQueue.front());
      stream->PacketQueue.pop();
      stream->CurrentPacketSerial = packet.Serial;
      if (prevSerial == INT32_MIN || stream->CurrentPacketSerial == INT32_MIN) {
        break;
      }
      if (prevSerial != stream->CurrentPacketSerial) {
        FormatContext.flush();
        if constexpr (MediaType == AVMEDIA_TYPE_AUDIO) {
          AudioPlayer->Stop();
        }
      }
      if (stream->PacketQueueSerial == stream->CurrentPacketSerial) {
        break;
      }
    }
    return packet;
  };

  auto isFrameQueueFull = [stream, &waitMutex]() {
    if (std::unique_lock frameLock(stream->FrameLock);
        stream->FrameQueue.size() > 60) {
      ImpLogSlow(LL_Trace, LC_Video, "%d FrameQueue full!\n", MediaType);
      std::unique_lock waitLock(waitMutex);
      frameLock.unlock();
      stream->DecodeCond.wait(waitLock);
      return true;
    }
    return false;
  };

  auto pushFrame = [stream](Frame_t<MediaType>&& frame) {
    AVFrameItem<MediaType> item;
    item.Frame = std::move(frame);
    if (!item.Frame) {
      item.Serial = INT32_MIN;
    } else {
      item.Serial = stream->PacketQueueSerial;
      item.Timestamp = item.Frame.pts();
    }
    std::lock_guard lock(stream->FrameLock);
    stream->FrameQueue.push(std::move(item));
  };

  while (!AbortRequest) {
    if (!hasPacket()) continue;
    if (isFrameQueueFull()) continue;

    AVPacketItem packet = verifyPacket();

    std::error_code ec;

    if (packet.Serial != INT32_MIN) {
      Frame_t<MediaType> frame = stream->CodecContext.decode(packet.Packet, ec);
      if (ec) {
        ImpLog(LL_Error, LC_Video, "Failed to decode %s", ec.message().c_str());
      }
      if (!frame) continue;  // Skip Empty Padding Frames
      pushFrame(std::move(frame));
    } else {
      // Flush decoder since packets are finished
      bool decode = true;
      while (decode) {
        if (ec) {
          ImpLog(LL_Error, LC_Video, "Failed to decode %s",
                 ec.message().c_str());
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
    ReadCond.notify_one();
    ReadThreadID.join();
    ReaderEOF = false;
    if (AudioStream) {
      AudioStream->DecodeCond.notify_one();
      AudioStream->DecoderThreadID.join();
      AudioStream.reset();
    }
    if (VideoStream) {
      VideoStream->DecodeCond.notify_one();
      VideoStream->DecoderThreadID.join();
      VideoStream.reset();
    }
    AudioPlayer->Unload();
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
    {
      std::unique_lock frameLock{VideoStream->FrameLock};
      size_t frameQueueSize = VideoStream->FrameQueue.size();
      if (frameQueueSize == 0) {
        frameLock.unlock();
        VideoStream->DecodeCond.notify_one();
        return;
      }
      AVFrameItem<AVMEDIA_TYPE_VIDEO> const& frame =
          VideoStream->FrameQueue.front();

      ImpLogSlow(LL_Trace, LC_Video, "VideoStream->FrameQueue.size() = %d\n",
                 frameQueueSize);
      SetFlag(SF_MOVIE_DRAWWAIT, false);

      if (frame.Serial == INT32_MIN) {
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
        size_t frameNum = av_rescale_q(frame.Timestamp.timestamp(),
                                       frame.Timestamp.timebase().getValue(),
                                       inverseFrameRate.getValue());
        // This isn't the place for it but I can't think of
        // anything right now
        ScrWork[SW_MOVIEFRAME] = (int)frameNum;
        duration = (frame.Timestamp.seconds() - PreviousFrameTimestamp);
      }
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

    VideoStream->FrameLock.lock();
    AVFrameItem<AVMEDIA_TYPE_VIDEO> frame =
        std::move(VideoStream->FrameQueue.front());
    VideoStream->FrameQueue.pop();
    VideoStream->FrameLock.unlock();
    FrameTimer += duration;
    if (duration > 0 && (time - FrameTimer) > 0.1) {
      FrameTimer = time;
    }
    PreviousFrameTimestamp = frame.Timestamp;

    VideoClock.Set(frame.Timestamp.seconds(), frame.Serial);
    VideoTexture->Submit(frame.Frame.data(0), frame.Frame.data(1),
                         frame.Frame.data(2));
    PlaybackStarted = true;
    VideoStream->DecodeCond.notify_one();
  }
}

void FFmpegPlayer::Render(float videoAlpha) {
  if (IsPlaying && PlaybackStarted) {
    float widthScale = Profile::DesignWidth / VideoTexture->Width;
    float heightScale = Profile::DesignHeight / VideoTexture->Height;
    glm::vec4 tint = glm::vec4(1.0f);
    tint.a = videoAlpha;
    Renderer->DrawVideoTexture(VideoTexture, glm::vec2(0.0f, 0.0f), tint,
                               glm::vec2(widthScale, heightScale), 0.0f,
                               IsAlpha);
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
  ImpLogSlow(LL_Trace, LC_Video, "Target delay: %f\n", duration);

  return duration;
}
}  // namespace Video
}  // namespace Impacto
