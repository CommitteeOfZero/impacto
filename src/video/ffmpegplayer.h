#pragma once

#define __STDC_CONSTANT_MACROS

extern "C" {
#include <libavutil/avutil.h>
}

#include "../impacto.h"
#include "../io/inputstream.h"
#include "../log.h"
#include "../renderer/yuvframe.h"
#include "../texture/texture.h"
#include "../renderer/renderer.h"
#include "videoplayer.h"
#include "alc.h"
#include "al.h"

// Forward Decl
struct SwrContext;
struct SwsContext;
struct AvMediaType;
struct AVFormatContext;
struct AVIOContext;

namespace Impacto {
namespace Io {
class InputStream;
}
}  // namespace Impacto

namespace Impacto {
namespace Video {

// Forward Decl
class FFmpegStream;

class Clock {
 public:
  double Pts;
  double PtsDrift;
  double LastUpdated;
  double Speed;
  int Serial;
  bool Paused;

  Clock();
  void SyncTo(Clock* target);
  void Set(double pts, int serial);
  double Get();
};

class FFmpegPlayer : public VideoPlayer {
 public:
  ~FFmpegPlayer();

  void Init() override;

  void Play(Io::InputStream* stream, bool loop, bool alpha) override;
  void Stop() override;
  void Seek(int64_t pos) override;

  void Update(float dt) override;
  void Render(float videoAlpha) override;

  void Read();
  void Decode(AVMediaType avType);
  void ProcessAudio();

  bool AbortRequest;
  bool SeekRequest;
  SDL_Thread* ReadThreadID;
  SDL_Thread* AudioThreadID;
  FFmpegStream* VideoStream = 0;
  FFmpegStream* AudioStream = 0;

 private:
  void FillAudioBuffers();
  double GetTargetDelay(double duration);
  bool QueuesHaveEnoughPackets();

  static int const FILESTREAMBUFFERSZ = 64 * 8192;
  uint8_t* FileStreamBuffer;
  SDL_cond* ReadCond;

  uint64_t Time;
  int64_t SeekPosition;

  AVFormatContext* FormatContext = 0;
  AVIOContext* IoContext = 0;
  SwsContext* ImgConvertContext;
  SwrContext* AudioConvertContext;
  Clock* VideoClock;
  Clock* ExternalClock;
  Clock* AudioClock;

  Clock* MasterClock;

  ALCuint ALSource;
  static int const AudioBufferSize = 256 * 8192;
  static int const AudioBufferCount = 3;
  ALuint BufferIds[AudioBufferCount];
  uint8_t HostBuffer[AudioBufferSize];
  int FirstFreeBuffer = 0;
  int FreeBufferCount = AudioBufferCount;
  int BufferStartPositions[AudioBufferCount];
  bool First = true;
  uint8_t** AudioBuffer;
  int AudioLinesize;

  bool IsInit = false;

  YUVFrame* VideoTexture;

  bool IsAlpha = false;
  bool Looping = false;
  bool ReaderEOF = false;
  bool PlaybackStarted = false;
  double PreviousFrameTimestamp = 0.0;
  double FrameTimer = 0.0;
  double MaxFrameDuration;
};

}  // namespace Video
}  // namespace Impacto
