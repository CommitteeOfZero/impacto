#pragma once

#include <codeccontext.h>
#include <stream.h>
#include <frame.h>
#include <packet.h>

#include "../impacto.h"
#include "clock.h"

#include <thread>
#include <memory>
#include <type_traits>
#if __SWITCH__ || __VITA__
#define __unix__
#endif
#include <readerwritercircularbuffer.h>
#if __SWITCH__ || __VITA__
#undef __unix__
#endif

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
  moodycamel::BlockingReaderWriterCircularBuffer<AVPacketItem> PacketQueue{25};
  moodycamel::BlockingReaderWriterCircularBuffer<AVFrameItem<MediaType>>
      FrameQueue{60};
  int Duration;
  int PacketQueueSerial = 0;
  int CurrentPacketSerial = 0;

  std::thread DecoderThread;

  FFmpegStream() = default;
  FFmpegStream(av::Stream&& avStream, DecodingContext_t<MediaType>&& codecCtx)
      : CodecContext(std::move(codecCtx)), stream(std::move(avStream)) {}

  FFmpegStream(av::Stream&& avStream, DecodingContext_t<MediaType>&& codecCtx,
               int duration)
      : CodecContext(std::move(codecCtx)),
        stream(std::move(avStream)),
        Duration(duration) {}

  void FlushPacketQueue();
  void FlushFrameQueue();
};

}  // namespace Video
}  // namespace Impacto
