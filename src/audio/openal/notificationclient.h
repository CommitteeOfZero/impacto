#pragma once
#ifdef _WIN32
#include <Windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

namespace Impacto {
namespace Audio {
namespace OpenAL {
    //I took this from stackoverflow
    //https://stackoverflow.com/questions/74965245/how-to-detect-default-audio-output-device-change-in-windows
class NotificationClient : public IMMNotificationClient {
 public:
  NotificationClient(bool* deviceChanged) {
    _deviceChanged = deviceChanged;
    Start();
  }

  ~NotificationClient() { Close(); }

  bool Start();

  void Close();

  // IUnknown methods
  STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) {
    if (riid == IID_IUnknown || riid == __uuidof(IMMNotificationClient)) {
      *ppvObject = static_cast<IMMNotificationClient*>(this);
      AddRef();
      return S_OK;
    }
    return E_NOINTERFACE;
  }

  ULONG STDMETHODCALLTYPE AddRef() { return InterlockedIncrement(&m_cRef); }

  ULONG STDMETHODCALLTYPE Release();

  // IMMNotificationClient methods
  STDMETHOD(OnDefaultDeviceChanged)(EDataFlow flow, ERole role,
                                    LPCWSTR pwstrDeviceId) {
    // Default audio device has been changed.
    *_deviceChanged = true;
    return S_OK;
  }

  STDMETHOD(OnDeviceAdded)(LPCWSTR pwstrDeviceId) {
    // A new audio device has been added.
    return S_OK;
  }

  STDMETHOD(OnDeviceRemoved)(LPCWSTR pwstrDeviceId) {
    // An audio device has been removed.
    return S_OK;
  }

  STDMETHOD(OnDeviceStateChanged)(LPCWSTR pwstrDeviceId, DWORD dwNewState) {
    // The state of an audio device has changed.
    return S_OK;
  }

  STDMETHOD(OnPropertyValueChanged)(LPCWSTR pwstrDeviceId,
                                    const PROPERTYKEY key) {
    // A property value of an audio device has changed.
    return S_OK;
  }

 private:
  bool* _deviceChanged;
  LONG m_cRef;
  IMMDeviceEnumerator* m_pEnumerator;
};
};  // namespace OpenAL
};  // namespace Audio
};  // namespace Impacto


#endif