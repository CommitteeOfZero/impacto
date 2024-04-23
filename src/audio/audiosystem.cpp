#include "audiosystem.h"
#include "../log.h"
#include "../profile/game.h"

#ifndef IMPACTO_DISABLE_OPENAL
#include "openal/audiobackend.h"
#include "openal/audiochannel.h"
#endif

#ifdef PLATFORM_DREAMCAST
#include "dreamcast/audiobackend.h"
#include "dreamcast/audiochannel.h"
#endif

#include <utility>

namespace Impacto {
namespace Audio {

static bool IsInit = false;

AudioBackend* Backend = 0;

float MasterVolume = 1.0f;
float GroupVolumes[ACG_Count];
AudioChannel* Channels[AC_Count];

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
#ifdef PLATFORM_DREAMCAST
    case AudioBackendType::Dreamcast: {
      Backend = new Dreamcast::AudioBackend();
      for (int i = 0; i < AC_Count; i++) {
        Channels[i] = new Dreamcast::AudioChannel();
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

  for (int i = 0; i < ACG_Count; i++) {
    GroupVolumes[i] = 0.5f;
  }

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
  for (int i = 0; i < AC_Count; i++) {
    Channels[i]->Update(dt);
  }
}

}  // namespace Audio
}  // namespace Impacto