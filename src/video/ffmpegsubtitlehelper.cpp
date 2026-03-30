#include "ffmpegsubtitlehelper.h"

#include <avcpp/packet.h>

namespace Impacto::Video {
// avcpp codeccontext.cpp
std::pair<int, const std::error_category*> make_error_pair(av::Errors errc) {
  return std::make_pair(static_cast<int>(errc), &av::avcpp_category());
}

// avcpp codeccontext.cpp
std::pair<int, const std::error_category*> make_error_pair(int status) {
  if (status < 0) return std::make_pair(status, &av::ffmpeg_category());
  return std::make_pair(status, nullptr);
}

void SubtitleData::setTimeBase(const av::Rational& value) {
  if (m_timeBase == value) return;

  int64_t rescaledPts = av::NoPts;

  if (m_timeBase != av::Rational() && value != av::Rational()) {
    if (m_raw.pts != av::NoPts)
      rescaledPts = m_timeBase.rescale(m_raw.pts, value);
  } else {
    rescaledPts = m_raw.pts;
  }

  if (m_timeBase != av::Rational()) {
    m_raw.pts = rescaledPts;
  }

  m_timeBase = value;
}

SubtitleDecoderContext::SubtitleDecoderContext(SubtitleDecoderContext&& other)
    : Parent(std::move(other)) {}

// avcpp codeccontext.cpp
std::pair<int, const std::error_category*> SubtitleDecoderContext::decodeCommon(
    AVSubtitle* outSub, const av::Packet& inPacket, size_t offset,
    int& frameFinished,
    int (*decodeProc)(AVCodecContext*, AVSubtitle*, int*,
                      const AVPacket*)) noexcept {
  if (!isValid()) return make_error_pair(av::Errors::CodecInvalid);

  if (!isOpened()) return make_error_pair(av::Errors::CodecNotOpened);

  if (!decodeProc) return make_error_pair(av::Errors::CodecInvalidDecodeProc);

  if (offset && inPacket.size() && offset >= inPacket.size())
    return make_error_pair(av::Errors::CodecDecodingOffsetToLarge);

  frameFinished = 0;

  AVPacket pkt = *inPacket.raw();
  pkt.data += offset;
  pkt.size -= static_cast<int>(offset);

  int decoded = decodeProc(m_raw, outSub, &frameFinished, &pkt);
  return make_error_pair(decoded);
}
std::pair<int, const std::error_category*> SubtitleDecoderContext::decodeCommon(
    SubtitleData& outSubtitle, const av::Packet& inPacket, size_t offset,
    int& frameFinished,
    int (*decodeProc)(AVCodecContext*, AVSubtitle*, int*,
                      const AVPacket*)) noexcept {
  auto st = decodeCommon(outSubtitle.raw(), inPacket, offset, frameFinished,
                         decodeProc);
  if (std::get<1>(st)) return st;

  if (!frameFinished) return std::make_pair(0u, nullptr);

  // Dial with PTS/DTS in packet/stream timebase

  if (inPacket.timeBase() != av::Rational())
    outSubtitle.setTimeBase(inPacket.timeBase());
#if AVCPP_HAS_AVFORMAT
  else
    outSubtitle.setTimeBase(stream().timeBase());
#endif  // if AVCPP_HAS_AVFORMAT

  AVSubtitle* subtitle = outSubtitle.raw();

  if (subtitle->pts == av::NoPts) subtitle->pts = inPacket.raw()->pts;

  // Convert to decoder/frame time base. Seems not nessesary.
  outSubtitle.setTimeBase(timeBase());

  if (inPacket) outSubtitle.setStreamIndex(inPacket.streamIndex());
#if AVCPP_HAS_AVFORMAT
  else
    outSubtitle.setStreamIndex(stream().index());
#endif  // if AVCPP_HAS_AVFORMAT

  outSubtitle.setComplete(true);

  return st;
}

SubtitleData SubtitleDecoderContext::decodeSubtitle(av::OptionalErrorCode ec,
                                                    const av::Packet& packet,
                                                    size_t offset,
                                                    size_t* decodedBytes) {
  clear_if(ec);

  SubtitleData outFrame;

  int gotFrame = 0;
  auto st = decodeCommon(outFrame, packet, offset, gotFrame,
                         [](AVCodecContext* avctx, AVSubtitle* subtitle,
                            int* got_picture_ptr, const AVPacket* avpkt) {
                           return avcodec_decode_subtitle2(
                               avctx, subtitle, got_picture_ptr, avpkt);
                         });

  if (get<1>(st)) {
    throws_if(ec, get<0>(st), *get<1>(st));
    return SubtitleData();
  }

  if (!gotFrame) return SubtitleData();

  if (decodedBytes) *decodedBytes = get<0>(st);

  return outFrame;
}

SubtitleData SubtitleDecoderContext::decode(const av::Packet& packet,
                                            av::OptionalErrorCode ec) {
  return decodeSubtitle(ec, packet, 0, nullptr);
}

SubtitleData SubtitleDecoderContext::decode(const av::Packet& packet,
                                            size_t offset, size_t& decodedBytes,
                                            av::OptionalErrorCode ec) {
  return decodeSubtitle(ec, packet, offset, &decodedBytes);
}

}  // namespace Impacto::Video