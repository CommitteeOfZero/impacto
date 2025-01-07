#pragma once

#include "audiocommon.h"

namespace Impacto {
namespace Audio {

class AudioBackend {
 public:
  virtual bool Init() { return true; };

  virtual void Shutdown() {};
};

}  // namespace Audio
}  // namespace Impacto