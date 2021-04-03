#pragma once

#include "videoplayer.h"
#include "../impacto.h"
#include "../io/inputstream.h"
#include "../log.h"

namespace Impacto {
namespace Video {

extern VideoPlayer Players[2];

void VideoInit();
void VideoUpdate(float dt);
void VideoRender();
void VideoShutdown();

}  // namespace Video
}  // namespace Impacto
