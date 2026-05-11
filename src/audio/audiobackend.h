#pragma once

#include "audiocommon.h"

namespace Impacto {
namespace Audio {

class AudioBackend {
 protected:
  bool deviceChanged = false;
 public:
  virtual bool Init() { return true; };

  virtual void Shutdown() {};

  virtual bool DeviceChanged() { return false; }
};

}  // namespace Audio
}  // namespace Impacto