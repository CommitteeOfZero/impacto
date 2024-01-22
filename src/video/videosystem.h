#pragma once

#include "videoplayer.h"
#include "../impacto.h"

namespace Impacto {
namespace Video {

enum VideoPlayerId { VP_Main = 0, VP_Secondary, VP_Count };

extern VideoPlayer* Players[VP_Count];

void VideoInit();
void VideoUpdate(float dt);
void VideoRender(float videoAlpha);
void VideoShutdown();

}  // namespace Video
}  // namespace Impacto
