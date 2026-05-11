#ifdef _WIN32
#include "notificationclient.h"
namespace Impacto {
namespace Audio {
namespace OpenAL {
bool NotificationClient::Start() {
  // Initialize the COM library for the current thread
  HRESULT ihr = CoInitialize(NULL);

  if (SUCCEEDED(ihr)) {
    // Create the device enumerator
    IMMDeviceEnumerator* pEnumerator;
    HRESULT hr =
        CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
                         __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (SUCCEEDED(hr)) {
      // Register for device change notifications
      hr = pEnumerator->RegisterEndpointNotificationCallback(this);
      m_pEnumerator = pEnumerator;

      return true;
    }

    CoUninitialize();
  }

  return false;
}

ULONG STDMETHODCALLTYPE NotificationClient::Release() {
  ULONG ulRef = InterlockedDecrement(&m_cRef);
  if (0 == ulRef) {
    delete this;
  }
  return ulRef;
}

void NotificationClient::Close() {
  // Unregister the device enumerator
  if (m_pEnumerator) {
    m_pEnumerator->UnregisterEndpointNotificationCallback(this);
    m_pEnumerator->Release();
  }

  // Uninitialize the COM library for the current thread
  CoUninitialize();
}
};  // namespace OpenAL
};  // namespace Audio
};  // namespace Impacto
#endif  // _WIN32
