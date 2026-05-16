#include "audiobackend.h"

#include "../../log.h"

namespace Impacto {
namespace Audio {
namespace OpenAL {

static ALCdevice* AlcDevice = 0;
static ALCcontext* AlcContext = 0;
static bool IsReopenSupported = false;
#if IMPACTO_OPENAL_HAVE_ALEXT
static LPALCEVENTCALLBACKSOFT EventCallBackFunc = 0;
static LPALCREOPENDEVICESOFT ReopenSoftFunc = 0;
static LPALCEVENTCONTROLSOFT EventControlFunc = 0;

void AL_APIENTRY DefaultDeviceChangedEventCallback(
    ALCenum eventType, ALCenum deviceType, ALCdevice* device, ALCsizei length,
    const ALCchar* message, void* userParam) {
  *(bool*)userParam = true;
}
#endif
bool AudioBackend::Init() {
  AlcDevice = alcOpenDevice(NULL);
  if (!AlcDevice) {
    ImpLog(LogLevel::Fatal, LogChannel::Audio,
           "Could not create OpenAL device\n");
    return false;
  }
  AlcContext = alcCreateContext(AlcDevice, NULL);
  if (!AlcContext || !alcMakeContextCurrent(AlcContext)) {
    ImpLog(LogLevel::Fatal, LogChannel::Audio,
           "Failed to create OpenAL context\n");
    alcCloseDevice(AlcDevice);
    return false;
  }
  if (alIsExtensionPresent("AL_EXT_float32") == AL_FALSE) {
    ImpLog(LogLevel::Error, LogChannel::Audio,
           "No floating-point audio support, some decoders may fail\n");
  }
#if IMPACTO_OPENAL_HAVE_ALEXT
  if (alcIsExtensionPresent(AlcDevice, "ALC_SOFT_system_events") == AL_TRUE) {
    if (alcIsExtensionPresent(AlcDevice, "ALC_SOFT_reopen_device") ==
        AL_FALSE) {
      ImpLog(LogLevel::Error, LogChannel::Audio,
             "No device reopen support,will be done manually\n");
    } else {
      IsReopenSupported = true;
      ReopenSoftFunc = (LPALCREOPENDEVICESOFT)alcGetProcAddress(
          AlcDevice, "alcReopenDeviceSOFT");
      if (!ReopenSoftFunc) IsReopenSupported = false;
    }

    EventControlFunc = (LPALCEVENTCONTROLSOFT)alcGetProcAddress(
        AlcDevice, "alcEventControlSOFT");
    if (EventControlFunc) {
      const ALCenum eventType{ALC_EVENT_TYPE_DEFAULT_DEVICE_CHANGED_SOFT};
      EventControlFunc(1, &eventType, AL_TRUE);
    }

    EventCallBackFunc = (LPALCEVENTCALLBACKSOFT)alcGetProcAddress(
        AlcDevice, "alcEventCallbackSOFT");
    if (EventCallBackFunc) {
      EventCallBackFunc((ALCEVENTPROCTYPESOFT)DefaultDeviceChangedEventCallback,
                        &deviceChanged);
    }
  }
#endif
  return true;
}

bool AudioBackend::Reinit() {
  deviceChanged = false;
#if IMPACTO_OPENAL_HAVE_ALEXT
  if (IsReopenSupported) {
    if (ReopenSoftFunc) {
      return ReopenSoftFunc(AlcDevice, NULL, NULL);
    } else {
      ImpLog(LogLevel::Error, LogChannel::Audio,
             "Reopen support is present but function was not found falling "
             "back to manual reopen\n");
    }
  }
#endif
  Shutdown();
  AlcDevice = alcOpenDevice(NULL);
  if (!AlcDevice) {
    ImpLog(LogLevel::Fatal, LogChannel::Audio,
           "Could not create OpenAL device\n");
    return false;
  }
  AlcContext = alcCreateContext(AlcDevice, NULL);
  if (!AlcContext || !alcMakeContextCurrent(AlcContext)) {
    ImpLog(LogLevel::Fatal, LogChannel::Audio,
           "Failed to create OpenAL context\n");
    alcCloseDevice(AlcDevice);
    return false;
  }
  return true;
}

void AudioBackend::Shutdown() {
  if (AlcContext) alcDestroyContext(AlcContext);
  if (AlcDevice) alcCloseDevice(AlcDevice);
}

bool AudioBackend::DeviceChanged() { return deviceChanged; }

bool AudioBackend::ReopenSupported() {
#if IMPACTO_OPENAL_HAVE_ALEXT
  return IsReopenSupported && ReopenSoftFunc && EventControlFunc &&
         EventCallBackFunc;
#else
  return false;
#endif
}

};  // namespace OpenAL
};  // namespace Audio
};  // namespace Impacto
