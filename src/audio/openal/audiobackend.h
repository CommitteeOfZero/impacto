#pragma once

#include "../audiobackend.h"
#include "audiocommon.h"

namespace Impacto {
namespace Audio {
namespace OpenAL {

class AudioBackend : public Audio::AudioBackend {
 private:
  bool DeviceChanged = false;
  void InitAlextFunctions();

 public:
  bool Init() override;

  bool Reinit() override;

  void Shutdown() override;

  bool DidDeviceChanged() override;

  bool ReopenSupported() override;
};

}  // namespace OpenAL
}  // namespace Audio
}  // namespace Impacto