#pragma once

#include <avcpp/codeccontext.h>
#include <avcpp/ffmpeg.h>

// Since these are basically extension classes for avcpp, we'll use their naming
// conventions for it for consistency

namespace Impacto::Subtitle {

struct SubtitleData {
  SubtitleData() = default;
  SubtitleData(AVSubtitle sub) : m_raw(sub) {};
  SubtitleData(SubtitleData&& other) {
    if (this == &other) return;
    m_raw = other.m_raw;
    m_timeBase = other.m_timeBase;
    m_streamIndex = other.m_streamIndex;
    m_isComplete = other.m_isComplete;

    // invalidate the moved-from object so it doesn't free the AVSubtitle
    other.m_raw.rects = nullptr;
  }
  SubtitleData(SubtitleData const& other) = delete;
  ~SubtitleData() {
    if (m_raw.rects) avsubtitle_free(&m_raw);
  }

  SubtitleData& operator=(SubtitleData const& other) = delete;
  SubtitleData& operator=(SubtitleData&& other) {
    if (this == &other) return *this;
    avsubtitle_free(&m_raw);
    m_raw = other.m_raw;
    m_timeBase = other.m_timeBase;
    m_streamIndex = other.m_streamIndex;
    m_isComplete = other.m_isComplete;

    // invalidate the moved-from object so it doesn't free the AVSubtitle
    other.m_raw.rects = nullptr;

    return *this;
  }

  AVSubtitle* raw() { return &m_raw; }

  void setTimeBase(const av::Rational& value);

  av::Timestamp pts() const { return {m_raw.pts, m_timeBase}; }

  void setPts(const av::Timestamp& ts) {
    if (m_timeBase == av::Rational()) m_timeBase = ts.timebase();
    m_raw.pts = ts.timestamp(m_timeBase);
  }

  const av::Rational& timeBase() const { return m_timeBase; }

  int streamIndex() const { return m_streamIndex; }

  void setStreamIndex(int streamIndex) { m_streamIndex = streamIndex; }

  void setComplete(bool isComplete) { m_isComplete = isComplete; }

  bool isComplete() const { return m_isComplete; }

  bool isValid() const { return m_raw.rects; }

  operator bool() const { return isValid() && isComplete(); }

  AVSubtitle m_raw{};

 protected:
  av::Rational m_timeBase{};
  int m_streamIndex{-1};
  bool m_isComplete{false};
};

// extension of avcpp, codeccontext.h
template <typename Clazz, av::Direction _direction>
class SubtitleCodecContext
    : public av::CodecContextBase<Clazz, _direction, AVMEDIA_TYPE_SUBTITLE> {
 public:
  using Parent = av::CodecContextBase<Clazz, _direction, AVMEDIA_TYPE_SUBTITLE>;

  using Parent::_log;
  using Parent::isOpened;
  using Parent::isValid;
  using Parent::Parent;

  std::string_view subtitleHeader() const {
    return std::string_view{reinterpret_cast<char*>(m_raw->subtitle_header),
                            static_cast<size_t>(m_raw->subtitle_header_size)};
  }

 protected:
  using Parent::m_raw;
  using Parent::moveOperator;
};

class SubtitleDecoderContext
    : public SubtitleCodecContext<SubtitleDecoderContext,
                                  av::Direction::Decoding> {
 public:
  using Parent =
      SubtitleCodecContext<SubtitleDecoderContext, av::Direction::Decoding>;
  using Parent::Parent;
  SubtitleDecoderContext() = default;
  SubtitleDecoderContext(SubtitleDecoderContext&& other);

  std::pair<int, const std::error_category*> decodeCommon(
      AVSubtitle* outSubtitle, const class av::Packet& inPacket, size_t offset,
      int& frameFinished,
      int (*decodeProc)(AVCodecContext*, AVSubtitle*, int*,
                        const AVPacket*)) noexcept;

  std::pair<int, const std::error_category*> decodeCommon(
      SubtitleData& outSubtitle, const class av::Packet& inPacket,
      size_t offset, int& frameFinished,
      int (*decodeProc)(AVCodecContext*, AVSubtitle*, int*,
                        const AVPacket*)) noexcept;

  SubtitleDecoderContext& operator=(SubtitleDecoderContext&& other);

  /**
   * @brief decode  - decode subtitle packet
   *
   * @param packet   packet to decode
   * @param[in,out] ec     this represents the error status on exit, if this is
   * pre-initialized to av#throws the function will throw on error instead
   * @return decoded subtitle frame, if error: exception thrown or error code
   * returns, in both cases output undefined.
   */
  SubtitleData decode(const av::Packet& packet,
                      av::OptionalErrorCode ec = av::throws());

  /**
   * @brief decode - decode subtitle packet with additional parameters
   *
   * @param[in] packet         packet to decode
   * @param[in] offset         data offset in packet
   * @param[out] decodedBytes  amount of decoded bytes
   * @param[in,out] ec     this represents the error status on exit, if this is
   * pre-initialized to av#throws the function will throw on error instead
   * @return decoded subtitle frame, if error: exception thrown or error code
   * returns, in both cases output undefined.
   */
  SubtitleData decode(const av::Packet& packet, size_t offset,
                      size_t& decodedBytes,
                      av::OptionalErrorCode ec = av::throws());

 private:
  SubtitleData decodeSubtitle(av::OptionalErrorCode ec,
                              const av::Packet& packet, size_t offset,
                              size_t* decodedBytes);
};

}  // namespace Impacto::Subtitle