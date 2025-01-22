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
inline AudioChannel* Channels[AC_Count];

inline bool VoiceMuted[VoiceCount];
inline float VoiceVolume[VoiceCount];

}  // namespace Audio
}  // namespace Impacto