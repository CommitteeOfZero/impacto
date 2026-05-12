#pragma once

#include "audiocommon.h"

namespace Impacto {
namespace Audio {

class AudioBackend {
 public:
  virtual bool Init() { return true; };

  virtual bool Reinit() { return true; };

  virtual void Shutdown() {};

  virtual bool DeviceChanged() { return false; };

  virtual bool ReopenSupported() { return false; }
};

}  // namespace Audio
}  // namespace Impacto