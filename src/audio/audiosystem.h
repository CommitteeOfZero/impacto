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
inline std::array<float, ACG_Count> GroupVolumes;
inline std::array<std::unique_ptr<AudioChannel>, AC_Count> Channels;

}  // namespace Audio
}  // namespace Impacto