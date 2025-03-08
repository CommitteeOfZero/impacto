#include "audiosystem.h"
#include "../log.h"
#include "../profile/game.h"
#include "../profile/scriptvars.h"
#include "../profile/configsystem.h"

#ifndef IMPACTO_DISABLE_OPENAL
#include "openal/audiobackend.h"
#include "openal/audiochannel.h"
#endif

#include <utility>

namespace Impacto {
namespace Audio {

using namespace Impacto::Profile::ScriptVars;

static bool IsInit = false;

void AudioShutdown() {
  Backend->Shutdown();
  IsInit = false;
  for (int i = 0; i < AC_Count; i++) {
    delete Channels[i];
    Channels[i] = 0;
  }
}

void AudioInit() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Audio, "Initialising audio system\n");

  switch (Profile::ActiveAudioBackend) {
#ifndef IMPACTO_DISABLE_OPENAL
    case AudioBackendType::OpenAL: {
      Backend = new OpenAL::AudioBackend();
      for (int i = 0; i < AC_Count; i++) {
        Channels[i] = new OpenAL::AudioChannel();
      }
    } break;
#endif
    default: {
      ImpLog(LL_Warning, LC_Audio,
             "Unknown or unsupported audio backend selected! You will not hear "
             "audio.\n");
      Backend = new AudioBackend();
      for (int i = 0; i < AC_Count; i++) {
        Channels[i] = new AudioChannel();
      }
    } break;
  }

  if (!Backend->Init()) return;

  for (int i = AC_SE0; i <= AC_SE2; i++)
    Channels[i]->Init((AudioChannelId)i, ACG_SE);
  for (int i = AC_VOICE0; i <= AC_REV; i++)
    Channels[i]->Init((AudioChannelId)i, ACG_Voice);
  for (int i = AC_BGM0; i <= AC_BGM2; i++)
    Channels[i]->Init((AudioChannelId)i, ACG_BGM);
  Channels[AC_SSE]->Init(AC_SSE, ACG_SE);

  IsInit = true;
}

void AudioUpdate(float dt) {
  // Set voice modifier for each voice channel
  for (int i = AC_VOICE0; i <= AC_VOICE2; i++) {
    const int charId = ScrWork[SW_ANIME0CHANO + (i - AC_VOICE0)];
    const int mappedCharId = ScrWork[SW_CHARACTERIDMAPPING + charId];
    const float voiceVolumeModifier =
        Profile::ConfigSystem::VoiceMuted[mappedCharId]
            ? 0.0f
            : Profile::ConfigSystem::VoiceVolume[mappedCharId];
    Channels[i]->Volume = voiceVolumeModifier;
  }

  for (int i = 0; i < AC_Count; i++) {
    Channels[i]->Update(dt);
  }
}

}  // namespace Audio
}  // namespace Impacto