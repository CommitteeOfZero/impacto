#pragma once

#include "../impacto.h"
#include "../io/inputstream.h"
#include "../log.h"
#include "videoplayer.h"

namespace Impacto {
namespace Video {

class NullPlayer : public VideoPlayer {
 public:
  void Init() override;

  void Play(Io::InputStream* stream, bool loop, bool alpha) override;
  void Stop() override;
  void Seek(int64_t pos) override;

  void Update(float dt) override;
  void Render(float videoAlpha) override;
};

}  // namespace Video
}  // namespace Impacto
