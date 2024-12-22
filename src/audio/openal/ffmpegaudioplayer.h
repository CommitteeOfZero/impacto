#pragma once

#include "audiocommon.h"

#include "../ffmpegaudioplayer.h"

namespace Impacto {
namespace Audio {
namespace OpenAL {

class FFmpegAudioPlayer : public Audio::FFmpegAudioPlayer {
 public:
  FFmpegAudioPlayer(Video::FFmpegPlayer* player)
      : Audio::FFmpegAudioPlayer(player) {}
  ~FFmpegAudioPlayer();

  void Init() override;
  void InitConvertContext(AVCodecContext* codecCtx) override;
  void FillAudioBuffers() override;
  void Process() override;

  void Stop() override;
  void Unload() override;

 private:
  ALCuint ALSource;
  static int constexpr AudioBufferSize = 256 * 8192;
  static int constexpr AudioBufferCount = 3;
  ALuint BufferIds[AudioBufferCount];
  uint8_t HostBuffer[AudioBufferSize];
  int FirstFreeBuffer = 0;
  int FreeBufferCount = AudioBufferCount;
  int BufferStartPositions[AudioBufferCount] = {};
  bool First = true;
};

}  // namespace OpenAL
}  // namespace Audio
}  // namespace Impacto
