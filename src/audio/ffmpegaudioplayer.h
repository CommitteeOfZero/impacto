#pragma once

#include "../video/ffmpegplayer.h"
#include "../video/ffmpegstream.h"

namespace Impacto {
namespace Audio {

class FFmpegAudioPlayer {
 public:
  FFmpegAudioPlayer(Video::FFmpegPlayer* player) { Player = player; }

  virtual void Init(){};
  virtual void InitConvertContext(AVCodecContext* codecCtx){};
  virtual void FillAudioBuffers(){};
  virtual void Process(){};

  virtual void Stop(){};
  virtual void Unload(){};

  Video::Clock* GetClock() { return AudioClock; };

 protected:
  uint8_t** AudioBuffer;
  int AudioLinesize;

  Video::Clock* AudioClock;

  SwrContext* AudioConvertContext;

  Video::FFmpegPlayer* Player;
};

}  // namespace Audio
}  // namespace Impacto
