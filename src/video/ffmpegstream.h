#pragma once

#include <mutex>
#include <codeccontext.h>
#include <stream.h>
#include <frame.h>
#include <packet.h>

#include "../impacto.h"
#include "clock.h"

#include <thread>
#include <memory>
#include <queue>
#include <type_traits>
#include <condition_variable>

struct AVFrame;
struct AVPacket;
struct AVCodecContext;
struct AVStream;

namespace Impacto {
namespace Video {

template <AVMediaType MediaType>
struct AVTypes {
  using type = void;
};

template <>
struct AVTypes<AVMEDIA_TYPE_VIDEO> {
  using DecodingContextType = av::VideoDecoderContext;
  using FrameType = av::VideoFrame;
};
template <>
struct AVTypes<AVMEDIA_TYPE_AUDIO> {
  using DecodingContextType = av::AudioDecoderContext;
  using FrameType = av::AudioSamples;
};

template <AVMediaType MediaType>
using DecodingContext_t = typename AVTypes<MediaType>::DecodingContextType;

template <AVMediaType MediaType>
using Frame_t = typename AVTypes<MediaType>::FrameType;

struct AVPacketItem {
  av::Packet Packet;
  int Serial = -1;
};

template <AVMediaType MediaType>
struct AVFrameItem {
  Frame_t<MediaType> Frame;
  int Serial = -1;
  av::Timestamp Timestamp;
};

template <AVMediaType MediaType>
struct FFmpegStream {
  DecodingContext_t<MediaType> CodecContext;
  av::Stream stream;
  std::queue<AVPacketItem> PacketQueue;
  std::queue<AVFrameItem<MediaType>> FrameQueue;
  int Duration;
  int PacketQueueSerial = 0;
  int CurrentPacketSerial = 0;

  std::thread DecoderThreadID;

  std::mutex PacketLock;
  std::mutex FrameLock;
  std::condition_variable DecodeCond;

  FFmpegStream() = default;
  FFmpegStream(av::Stream&& avStream, DecodingContext_t<MediaType>&& codecCtx)
      : CodecContext(std::move(codecCtx)), stream(std::move(avStream)) {}

  FFmpegStream(av::Stream&& avStream, DecodingContext_t<MediaType>&& codecCtx,
               int duration)
      : CodecContext(std::move(codecCtx)),
        stream(std::move(avStream)),
        Duration(duration) {}

  ~FFmpegStream() {
    FlushFrameQueue();
    FlushPacketQueue();
  }

  void FlushPacketQueue();
  void FlushFrameQueue();
};

}  // namespace Video
}  // namespace Impacto
