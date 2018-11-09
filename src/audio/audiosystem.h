#pragma once

#include "../impacto.h"
#include "../log.h"
#include "audiochannel.h"

namespace Impacto {
namespace Audio {

void AudioInit();
void AudioUpdate(float dt);
void AudioShutdown();

extern float MasterVolume;
extern float GroupVolumes[ACG_Count];
extern AudioChannel Channels[AC_Count];

}  // namespace Audio
}  // namespace Impacto