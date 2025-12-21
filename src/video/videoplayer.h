#pragma once

#include "../impacto.h"
#include "../io/stream.h"

#include <vector>

namespace Impacto {
namespace Video {

class VideoPlayer {
 public:
  static VideoPlayer* Create(Io::Stream* stream);

  virtual void Init() {};
  virtual void Play(Io::Stream* stream, bool loop, bool alpha) {};
  virtual void Stop() {};
  virtual void Seek(int64_t pos) {};

  virtual void Update(float dt) {};
  virtual void Render(float videoAlpha) {};

  bool IsPlaying = false;

  bool CancelFlag = false;
  uint8_t CancelWaitTime = 0;

 protected:
  using VideoPlayerCreator = auto (*)(Io::Stream* stream) -> VideoPlayer*;
  static bool AddVideoPlayerCreator(VideoPlayerCreator c);

 private:
  static std::vector<VideoPlayerCreator> Registry;
};

}  // namespace Video
}  // namespace Impacto