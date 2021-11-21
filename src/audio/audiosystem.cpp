#include "audiosystem.h"
#include "../log.h"
#include <utility>

namespace Impacto {
namespace Audio {

static bool IsInit = false;

static ALCdevice* AlcDevice = 0;
static ALCcontext* AlcContext = 0;

float MasterVolume = 1.0f;
float GroupVolumes[ACG_Count];
AudioChannel Channels[AC_Count];

void AudioShutdown() {
  if (AlcContext) alcDestroyContext(AlcContext);
  if (AlcDevice) alcCloseDevice(AlcDevice);
  IsInit = false;
  for (int i = 0; i < AC_Count; i++) {
    Channels[i] = AudioChannel();
  }
}

void AudioInit() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Audio, "Initialising audio system\n");

  AlcDevice = alcOpenDevice(NULL);
  if (!AlcDevice) {
    ImpLog(LL_Fatal, LC_Audio, "Could not create OpenAL device\n");
    return;
  }
  if (alIsExtensionPresent("AL_EXT_float32") == AL_FALSE) {
    ImpLog(LL_Error, LC_Audio,
           "No floating-point audio support, some decoders may fail\n");
  }
  AlcContext = alcCreateContext(AlcDevice, NULL);
  if (!AlcContext || !alcMakeContextCurrent(AlcContext)) {
    ImpLog(LL_Fatal, LC_Audio, "Failed to create OpenAL context\n");
    alcCloseDevice(AlcDevice);
    return;
  }

  for (int i = 0; i < ACG_Count; i++) {
    GroupVolumes[i] = 0.5f;
  }

  for (int i = AC_SE0; i <= AC_SE2; i++)
    Channels[i].Init((AudioChannelId)i, ACG_SE);
  for (int i = AC_VOICE0; i <= AC_REV; i++)
    Channels[i].Init((AudioChannelId)i, ACG_Voice);
  for (int i = AC_BGM0; i <= AC_BGM2; i++)
    Channels[i].Init((AudioChannelId)i, ACG_BGM);
  Channels[AC_SSE].Init(AC_SSE, ACG_SE);

  IsInit = true;
}

void AudioUpdate(float dt) {
  for (int i = 0; i < AC_Count; i++) {
    Channels[i].Update(dt);
  }
}

}  // namespace Audio
}  // namespace Impacto