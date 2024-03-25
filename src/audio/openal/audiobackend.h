#pragma once

#include "../audiobackend.h"
#include "audiocommon.h"

namespace Impacto {
namespace Audio {
namespace OpenAL {

class AudioBackend : public Audio::AudioBackend {
 public:
  bool Init() override;

  void Shutdown() override;
};

}  // namespace OpenAL
}  // namespace Audio
}  // namespace Impacto