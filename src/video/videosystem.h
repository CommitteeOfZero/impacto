#pragma once

#include "videoplayer.h"
#include "../impacto.h"

namespace Impacto {
namespace Video {

extern VideoPlayer Players[2];

void VideoInit();
void VideoUpdate(float dt);
void VideoRender(float videoAlpha);
void VideoShutdown();

}  // namespace Video
}  // namespace Impacto
