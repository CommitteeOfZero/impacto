#include "videosystem.h"
#include "../log.h"
#include <utility>

namespace Impacto {
namespace Video {

VideoPlayer Players[2];

void VideoShutdown() {
  for (int i = 0; i < 2; i++) {
    Players[i].Stop();
  }
}

void VideoInit() {
  ImpLog(LL_Info, LC_Audio, "Initialising video system\n");
  for (int i = 0; i < 2; i++) {
    Players[i].Init();
  }
}

void VideoUpdate(float dt) {
  for (int i = 0; i < 2; i++) {
    Players[i].Update(dt);
  }
}

void VideoRender(float videoAlpha) {
  for (int i = 0; i < 2; i++) {
    Players[i].Render(videoAlpha);
  }
}

}  // namespace Video
}  // namespace Impacto
