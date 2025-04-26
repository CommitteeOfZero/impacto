#pragma once

#include "../impacto.h"
#include "../log.h"
#include "audiochannel.h"
#include "audiobackend.h"

namespace Impacto {
namespace Audio {

void AudioInit();
void AudioUpdate(float dt);
void AudioShutdown();

inline AudioBackend* Backend = nullptr;

inline float MasterVolume = 1.0f;
inline float GroupVolumes[ACG_Count];
inline std::unique_ptr<AudioChannel> Channels[AC_Count];

}  // namespace Audio
}  // namespace Impacto