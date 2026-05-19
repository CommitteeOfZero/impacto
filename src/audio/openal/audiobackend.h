#pragma once

#include "../audiobackend.h"
#include "audiocommon.h"

namespace Impacto {
namespace Audio {
namespace OpenAL {

class AudioBackend : public Audio::AudioBackend {
 private:
  bool deviceChanged = false;
  void InitAlextFunctions();

 public:
  bool Init() override;

  bool Reinit() override;

  void Shutdown() override;

  bool DeviceChanged() override;

  bool ReopenSupported() override;
};

}  // namespace OpenAL
}  // namespace Audio
}  // namespace Impacto