#include "audiobackend.h"

#include "../../log.h"

namespace Impacto {
namespace Audio {
namespace OpenAL {

static ALCdevice* AlcDevice = 0;
static ALCcontext* AlcContext = 0;
static bool IsReopenSupported = false;
#if ALC_SOFT_system_events
static LPALCEVENTCALLBACKSOFT EventCallBackFunc = 0;
static LPALCEVENTCONTROLSOFT EventControlFunc = 0;
#endif
#if ALC_SOFT_reopen_device
static LPALCREOPENDEVICESOFT ReopenSoftFunc = 0;
#endif
void AL_APIENTRY DefaultDeviceChangedEventCallback(
    ALCenum eventType, ALCenum deviceType, ALCdevice* device, ALCsizei length,
    const ALCchar* message, void* userParam) {
  *(bool*)userParam = true;
}

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
  InitAlextFunctions();
  return true;
}

bool AudioBackend::Reinit() {
  deviceChanged = false;
#if ALC_SOFT_reopen_device
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
#if ALC_SOFT_reopen_device
  return IsReopenSupported && ReopenSoftFunc && EventControlFunc &&
         EventCallBackFunc;
#else
  return false;
#endif
}

void AudioBackend::InitAlextFunctions() {
#if ALC_SOFT_system_events
  if (alcIsExtensionPresent(AlcDevice, "ALC_SOFT_system_events") == AL_TRUE) {
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
#if ALC_SOFT_reopen_device
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
#endif
  }
#endif
}

};  // namespace OpenAL
};  // namespace Audio
};  // namespace Impacto
