#include "audiobackend.h"
#ifdef _WIN32
#include "notificationclient.h"
#endif
#include "../../log.h"

namespace Impacto {
namespace Audio {
namespace OpenAL {

static ALCdevice* AlcDevice = 0;
static ALCcontext* AlcContext = 0;
#ifdef _WIN32
static NotificationClient* NotifClient = 0;
#endif

bool AudioBackend::Init() {
  deviceChanged = false;
  AlcDevice = alcOpenDevice(NULL);
  if (!AlcDevice) {
    ImpLog(LogLevel::Fatal, LogChannel::Audio,
           "Could not create OpenAL device\n");
    return false;
  }
  if (alIsExtensionPresent("AL_EXT_float32") == AL_FALSE) {
    ImpLog(LogLevel::Error, LogChannel::Audio,
           "No floating-point audio support, some decoders may fail\n");
  }
  AlcContext = alcCreateContext(AlcDevice, NULL);
  if (!AlcContext || !alcMakeContextCurrent(AlcContext)) {
    ImpLog(LogLevel::Fatal, LogChannel::Audio,
           "Failed to create OpenAL context\n");
    alcCloseDevice(AlcDevice);
    return false;
  }
#ifdef _WIN32
  if (!NotifClient) NotifClient = new NotificationClient(&deviceChanged);
#endif
  return true;
}

void AudioBackend::Shutdown() {
  if (AlcContext) alcDestroyContext(AlcContext);
  if (AlcDevice) alcCloseDevice(AlcDevice);
}

bool AudioBackend::DeviceChanged() { return deviceChanged; }
};  // namespace OpenAL
};  // namespace Audio
};  // namespace Impacto
