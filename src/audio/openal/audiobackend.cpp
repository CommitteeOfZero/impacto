#include "audiobackend.h"

#include "../../log.h"

namespace Impacto {
namespace Audio {
namespace OpenAL {

static ALCdevice* AlcDevice = 0;
static ALCcontext* AlcContext = 0;

bool AudioBackend::Init() {
  AlcDevice = alcOpenDevice(NULL);
  if (!AlcDevice) {
    ImpLog(LL_Fatal, LC_Audio, "Could not create OpenAL device\n");
    return false;
  }
  if (alIsExtensionPresent("AL_EXT_float32") == AL_FALSE) {
    ImpLog(LL_Error, LC_Audio,
           "No floating-point audio support, some decoders may fail\n");
  }
  AlcContext = alcCreateContext(AlcDevice, NULL);
  if (!AlcContext || !alcMakeContextCurrent(AlcContext)) {
    ImpLog(LL_Fatal, LC_Audio, "Failed to create OpenAL context\n");
    alcCloseDevice(AlcDevice);
    return false;
  }
  return true;
}

void AudioBackend::Shutdown() {
  if (AlcContext) alcDestroyContext(AlcContext);
  if (AlcDevice) alcCloseDevice(AlcDevice);
}

};  // namespace OpenAL
};  // namespace Audio
};  // namespace Impacto
