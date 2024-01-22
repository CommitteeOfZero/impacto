#pragma once

#include "../impacto.h"
#include "../io/inputstream.h"

#include <vector>

namespace Impacto {
namespace Video {

class VideoPlayer {
 public:
  static VideoPlayer* Create(Io::InputStream* stream);

  virtual void Init() = 0;
  virtual void Play(Io::InputStream* stream, bool loop, bool alpha) = 0;
  virtual void Stop() = 0;
  virtual void Seek(int64_t pos) = 0;

  virtual void Update(float dt) = 0;
  virtual void Render(float videoAlpha) = 0;

  bool IsPlaying;

 protected:
  typedef VideoPlayer* (*VideoPlayerCreator)(Io::InputStream* stream);
  static bool AddVideoPlayerCreator(VideoPlayerCreator c);

 private:
  static std::vector<VideoPlayerCreator> Registry;
};

}  // namespace Video
}  // namespace Impacto