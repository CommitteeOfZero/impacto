#pragma once
#include <chrono>
#include <condition_variable>
#include <memory>
#include <thread>
#include <optional>

#include <avcpp/formatcontext.h>
#include <avcpp/timestamp.h>

#include "clock.h"
#include "videoplayer.h"
#include "ffmpegstream.h"
#include "../io/stream.h"
#include "../renderer/yuvframe.h"
#include "../audio/ffmpegaudioplayer.h"
#include "../subtitle/subtitlesystem.h"

namespace Impacto {
namespace Io {
class Stream;
}

namespace Video {
struct FFmpegFileIO : public av::CustomIO {
  FFmpegFileIO() = default;
  FFmpegFileIO(Io::Stream* Stream) : FileStream(Stream) {}
  Io::Stream* FileStream;
  int read(uint8_t* data, size_t size) override;
  int64_t seek(int64_t offset, int whence) override;
  int seekable() const override {
    return FileStream != nullptr ? AVIO_SEEKABLE_NORMAL : 0;
  }
};

class FFmpegPlayer : public VideoPlayer {
 public:
  ~FFmpegPlayer();

  void Init() override;

  void Play(Io::Stream* stream, bool loop, bool alpha) override;
  void Stop() override;
  void Seek(int64_t pos) override;

  void Update(float dt) override;
  void Render(float videoAlpha) override;

  void Read();
  template <AVMediaType avType>
  void Decode(FFmpegStream<avType>& stream);
  void ProcessAudio();

  std::atomic<bool> AbortRequest;
  bool SeekRequest;
  std::thread ReadThread;
  std::optional<FFmpegStream<AVMEDIA_TYPE_VIDEO>> VideoStream;
  std::optional<FFmpegStream<AVMEDIA_TYPE_AUDIO>> AudioStream;

  std::vector<FFmpegStream<AVMEDIA_TYPE_SUBTITLE>> EmbeddedSubtitleStreams;
  std::optional<Subtitle::SubtitlePlayer> SubPlayer;

 private:
  void InitSubtitles(std::vector<std::pair<av::Stream, int>>& subtitleStreams);
  void FillAudioBuffers();
  Clock::Microseconds GetTargetDelay(Clock::Microseconds duration);
  bool QueuesHaveEnoughPackets();

  void HandleSeekRequest();

  template <AVMediaType MediaType>
  void OpenCodec(std::optional<FFmpegStream<MediaType>>& streamOpt,
                 av::Stream&& avStream, int streamId);

  void UpdateSubtitles();
  void ProcessVideoFrame(Frame_t<AVMEDIA_TYPE_VIDEO>& avFrame);
  AVBufferRef* HwDecoderInit(const AVCodec* codec);

  static int constexpr FILESTREAMBUFFERSZ = 64 * 8192;
  std::condition_variable ReadCond;

  uint64_t Time;
  int64_t SeekPosition;

  std::unique_ptr<Io::Stream> StreamPtr;
  av::FormatContext FormatContext;
  AVPixelFormat HwVideoPixelFormat = AV_PIX_FMT_NONE;
  FFmpegFileIO IoContext;

  Clock VideoClock;
  Clock* MasterClock{};

  std::unique_ptr<Audio::FFmpegAudioPlayer> AudioPlayer;

  bool IsInit = false;

  std::variant<std::monostate, YUVFrame*, NV12Frame*> VideoTexture;

  bool IsAlpha = false;
  bool Looping = false;
  bool ReaderEOF = false;
  bool PlaybackStarted = false;
  std::optional<av::Timestamp> PreviousFrameTimestamp{};
  Clock::MonotonicTime FrameTimer{};
  Clock::Microseconds MaxFrameDuration{};
  bool NoAudio = false;
  int FrameCount = 0;
};

}  // namespace Video
}  // namespace Impacto
