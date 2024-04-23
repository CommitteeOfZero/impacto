#pragma once

#include "../audiobackend.h"

namespace Impacto {
namespace Audio {
namespace Dreamcast {

class AudioBackend : public Audio::AudioBackend {
 public:
  bool Init() override;

  void Shutdown() override;
};

}  // namespace Dreamcast
}  // namespace Audio
}  // namespace Impacto