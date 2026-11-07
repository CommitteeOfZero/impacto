#include "audiobackend.h"

#include "../../log.h"

#include <array>

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
#if ALC_EXT_thread_local_context
static PFNALCSETTHREADCONTEXTPROC SetThreadContextFunc = 0;
static PFNALCGETTHREADCONTEXTPROC GetThreadContextFunc = 0;
#endif

struct OutputDevice {
  ALCdevice* Device = 0;
  ALCcontext* Context = 0;
};
static std::array<OutputDevice, AO_Count> Outputs;

#if ALC_SOFT_system_events
void AL_APIENTRY AudioBackend::SystemEventCallback(
    ALCenum eventType, ALCenum deviceType, ALCdevice* device, ALCsizei length,
    const ALCchar* message, void* userParam) {
  if (deviceType != ALC_PLAYBACK_DEVICE_SOFT) return;

  AudioBackend* backend = static_cast<AudioBackend*>(userParam);
  switch (eventType) {
    case ALC_EVENT_TYPE_DEFAULT_DEVICE_CHANGED_SOFT:
      backend->DeviceChanged = true;
      break;
    case ALC_EVENT_TYPE_DEVICE_ADDED_SOFT:
    case ALC_EVENT_TYPE_DEVICE_REMOVED_SOFT:
      backend->DeviceListChanged = true;
      break;
  }
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
  InitAlextFunctions();
  return true;
}

bool AudioBackend::Reinit() {
  DeviceChanged = false;
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
  if (AlcContext) alcDestroyContext(AlcContext);
  if (AlcDevice) alcCloseDevice(AlcDevice);
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
  for (int i = 0; i < AO_Count; i++) {
    CloseOutput((AudioOutputId)i);
  }
  if (AlcContext) alcDestroyContext(AlcContext);
  if (AlcDevice) alcCloseDevice(AlcDevice);
}

bool AudioBackend::DidDeviceChanged() { return DeviceChanged; }

bool AudioBackend::ReopenSupported() {
#if ALC_SOFT_reopen_device
  return IsReopenSupported && ReopenSoftFunc && EventControlFunc &&
         EventCallBackFunc;
#else
  return false;
#endif
}

std::vector<std::string> AudioBackend::GetPlaybackDevices() {
  ALCenum specifier = ALC_DEVICE_SPECIFIER;
#if ALC_ENUMERATE_ALL_EXT
  if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT") == AL_TRUE) {
    specifier = ALC_ALL_DEVICES_SPECIFIER;
  }
#endif

  std::vector<std::string> devices;
  for (const ALCchar* name = alcGetString(NULL, specifier); name && *name;
       name += devices.back().size() + 1) {
    devices.emplace_back(name);
  }
  return devices;
}

bool AudioBackend::DidDeviceListChanged() {
  return DeviceListChanged.exchange(false);
}

bool AudioBackend::OpenOutput(AudioOutputId output,
                              std::string const& deviceName) {
  if (output == AO_Main) return false;

#if ALC_EXT_thread_local_context
  if (!SetThreadContextFunc || !GetThreadContextFunc) {
    ImpLog(LogLevel::Error, LogChannel::Audio,
           "No thread local context support, cannot open audio output on "
           "{:s}\n",
           deviceName);
    return false;
  }

  CloseOutput(output);
  OutputDevice& outputDevice = Outputs[output];
  outputDevice.Device = alcOpenDevice(deviceName.c_str());
  if (!outputDevice.Device) {
    ImpLog(LogLevel::Error, LogChannel::Audio,
           "Could not open OpenAL device {:s}\n", deviceName);
    return false;
  }
  outputDevice.Context = alcCreateContext(outputDevice.Device, NULL);
  if (!outputDevice.Context) {
    ImpLog(LogLevel::Error, LogChannel::Audio,
           "Failed to create OpenAL context on {:s}\n", deviceName);
    CloseOutput(output);
    return false;
  }
  return true;
#else
  return false;
#endif
}

void AudioBackend::CloseOutput(AudioOutputId output) {
  if (output == AO_Main) return;

  OutputDevice& outputDevice = Outputs[output];
  if (outputDevice.Context) alcDestroyContext(outputDevice.Context);
  if (outputDevice.Device) alcCloseDevice(outputDevice.Device);
  outputDevice = {};
}

bool AudioBackend::IsOutputOpen(AudioOutputId output) {
  if (output == AO_Main) return AlcContext != 0;

  OutputDevice const& outputDevice = Outputs[output];
  if (!outputDevice.Context) return false;
#if ALC_EXT_disconnect
  ALCint connected = AL_TRUE;
  alcGetIntegerv(outputDevice.Device, ALC_CONNECTED, 1, &connected);
  return connected == AL_TRUE;
#else
  return true;
#endif
}

void AudioBackend::InitAlextFunctions() {
#if ALC_EXT_thread_local_context
  if (alcIsExtensionPresent(NULL, "ALC_EXT_thread_local_context") == AL_TRUE) {
    SetThreadContextFunc = (PFNALCSETTHREADCONTEXTPROC)alcGetProcAddress(
        NULL, "alcSetThreadContext");
    GetThreadContextFunc = (PFNALCGETTHREADCONTEXTPROC)alcGetProcAddress(
        NULL, "alcGetThreadContext");
  }
#endif
#if ALC_SOFT_system_events
  if (alcIsExtensionPresent(AlcDevice, "ALC_SOFT_system_events") == AL_TRUE) {
    EventControlFunc = (LPALCEVENTCONTROLSOFT)alcGetProcAddress(
        AlcDevice, "alcEventControlSOFT");
    if (EventControlFunc) {
      const std::array<ALCenum, 3> eventTypes{
          ALC_EVENT_TYPE_DEFAULT_DEVICE_CHANGED_SOFT,
          ALC_EVENT_TYPE_DEVICE_ADDED_SOFT,
          ALC_EVENT_TYPE_DEVICE_REMOVED_SOFT,
      };
      EventControlFunc((ALCsizei)eventTypes.size(), eventTypes.data(), AL_TRUE);
    }

    EventCallBackFunc = (LPALCEVENTCALLBACKSOFT)alcGetProcAddress(
        AlcDevice, "alcEventCallbackSOFT");
    if (EventCallBackFunc) {
      EventCallBackFunc((ALCEVENTPROCTYPESOFT)SystemEventCallback, this);
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

ScopedOutputContext::ScopedOutputContext(AudioOutputId output) {
#if ALC_EXT_thread_local_context
  if (!SetThreadContextFunc || !GetThreadContextFunc) return;

  ALCcontext* context = output == AO_Main ? nullptr : Outputs[output].Context;
  PreviousContext = GetThreadContextFunc();
  if (context == PreviousContext) return;
  Bound = SetThreadContextFunc(context) == AL_TRUE;
#endif
}

ScopedOutputContext::~ScopedOutputContext() {
#if ALC_EXT_thread_local_context
  if (Bound) SetThreadContextFunc(PreviousContext);
#endif
}

};  // namespace OpenAL
};  // namespace Audio
};  // namespace Impacto
