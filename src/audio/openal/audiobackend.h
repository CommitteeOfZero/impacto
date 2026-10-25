#pragma once

#include "../audiobackend.h"
#include "audiocommon.h"

#include <atomic>

namespace Impacto {
namespace Audio {
namespace OpenAL {

class AudioBackend : public Audio::AudioBackend {
 private:
  std::atomic<bool> DeviceChanged = false;
  std::atomic<bool> DeviceListChanged = false;
  void InitAlextFunctions();

#if ALC_SOFT_system_events
  static void AL_APIENTRY
  SystemEventCallback(ALCenum eventType, ALCenum deviceType, ALCdevice* device,
                      ALCsizei length, const ALCchar* message, void* userParam);
#endif

 public:
  bool Init() override;

  bool Reinit() override;

  void Shutdown() override;

  bool DidDeviceChanged() override;

  bool ReopenSupported() override;

  std::vector<std::string> GetPlaybackDevices() override;

  bool DidDeviceListChanged() override;

  bool OpenOutput(AudioOutputId output, std::string const& deviceName) override;

  void CloseOutput(AudioOutputId output) override;

  bool IsOutputOpen(AudioOutputId output) override;
};

class ScopedOutputContext {
 public:
  explicit ScopedOutputContext(AudioOutputId output);
  ~ScopedOutputContext();

  ScopedOutputContext(ScopedOutputContext const&) = delete;
  ScopedOutputContext& operator=(ScopedOutputContext const&) = delete;

 private:
  bool Bound = false;
  ALCcontext* PreviousContext = nullptr;
};

}  // namespace OpenAL
}  // namespace Audio
}  // namespace Impacto