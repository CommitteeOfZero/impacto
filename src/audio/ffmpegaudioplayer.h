#pragma once

#include "../video/ffmpegstream.h"
#include "../video/clock.h"

namespace Impacto {

namespace Video {
class FFmpegPlayer;
}
namespace Audio {

class FFmpegAudioPlayer {
 public:
  FFmpegAudioPlayer(Video::FFmpegPlayer* player) : Player(player) {}
  virtual ~FFmpegAudioPlayer() = default;

  virtual void Init(){};
  virtual void InitConvertContext(AVCodecContext* codecCtx){};
  virtual void FillAudioBuffers(){};
  virtual void Process(){};

  virtual void Stop(){};
  virtual void Unload(){};

  Video::Clock& GetClock() { return AudioClock; };

 protected:
  uint8_t** AudioBuffer = 0;
  int AudioLinesize;

  Video::Clock AudioClock;

  SwrContext* AudioConvertContext = 0;

  Video::FFmpegPlayer* Player;
};

}  // namespace Audio
}  // namespace Impacto
