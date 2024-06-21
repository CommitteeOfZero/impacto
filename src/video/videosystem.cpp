#include "videosystem.h"
#ifndef IMPACTO_DISABLE_FFMPEG
#include "ffmpegplayer.h"
#endif
#include "../profile/game.h"
#include "../log.h"

namespace Impacto {
namespace Video {

void VideoShutdown() {
  for (int i = 0; i < VP_Count; i++) {
    Players[i]->Stop();
  }
}

void VideoInit() {
  ImpLog(LL_Info, LC_Video, "Initialising video system\n");

  switch (Profile::VideoPlayer) {
#ifndef IMPACTO_DISABLE_FFMPEG
    case VideoPlayerType::FFmpeg: {
      for (int i = 0; i < VP_Count; i++) {
        Players[i] = new FFmpegPlayer();
        Players[i]->Init();
      }
    } break;
#endif
    default: {
      ImpLog(LL_Warning, LC_Video,
             "No suitable video player found! Using a null one, which means "
             "you will not see any videos.\n");
      for (int i = 0; i < VP_Count; i++) {
        Players[i] = new VideoPlayer();
      }
    } break;
  }
}

void VideoUpdate(float dt) {
  for (int i = 0; i < VP_Count; i++) {
    Players[i]->Update(dt);
  }
}

void VideoRender(float videoAlpha) {
  for (int i = 0; i < VP_Count; i++) {
    Players[i]->Render(videoAlpha);
  }
}

}  // namespace Video
}  // namespace Impacto
