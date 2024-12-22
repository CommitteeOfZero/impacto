#pragma once
#include <formatcontext.h>
#include <timestamp.h>
#include <libavutil/avutil.h>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <thread>
#include <optional>
#include "../io/stream.h"
#include "../renderer/yuvframe.h"
#include "videoplayer.h"
#include "../audio/ffmpegaudioplayer.h"
#include "ffmpegstream.h"
#include "clock.h"

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
  void Decode();
  void ProcessAudio();

  bool AbortRequest;
  bool SeekRequest;
  std::thread ReadThreadID;
  std::optional<FFmpegStream<AVMEDIA_TYPE_VIDEO>> VideoStream;
  std::optional<FFmpegStream<AVMEDIA_TYPE_AUDIO>> AudioStream;

 private:
  void FillAudioBuffers();
  double GetTargetDelay(double duration);
  bool QueuesHaveEnoughPackets();

  void HandleSeekRequest();

  template <AVMediaType MediaType>
  void OpenCodec(std::optional<FFmpegStream<MediaType>>& streamOpt,
                 av::Stream&& avStream, int streamId);

  static int constexpr FILESTREAMBUFFERSZ = 64 * 8192;
  std::condition_variable ReadCond;

  uint64_t Time;
  int64_t SeekPosition;

  std::unique_ptr<Io::Stream> StreamPtr;
  av::FormatContext FormatContext;
  FFmpegFileIO IoContext;
  SwsContext* ImgConvertContext;
  Clock VideoClock;

  Clock* MasterClock{};

  std::unique_ptr<Audio::FFmpegAudioPlayer> AudioPlayer;

  bool IsInit = false;

  YUVFrame* VideoTexture;

  bool IsAlpha = false;
  bool Looping = false;
  bool ReaderEOF = false;
  bool PlaybackStarted = false;
  double PreviousFrameTimestamp = 0.0;
  double FrameTimer = 0.0;
  double MaxFrameDuration = 0.0;
  bool NoAudio = false;
  int FrameCount = 0;
};

}  // namespace Video
}  // namespace Impacto
