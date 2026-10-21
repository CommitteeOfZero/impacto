#pragma once

#include "audiocommon.h"

#include <string>
#include <vector>

namespace Impacto {
namespace Audio {

class AudioBackend {
 public:
  virtual bool Init() { return true; };

  virtual bool Reinit() { return true; };

  virtual void Shutdown() {};

  virtual bool DidDeviceChanged() { return false; };

  virtual bool ReopenSupported() { return false; }

  virtual std::vector<std::string> GetPlaybackDevices() { return {}; }

  virtual bool DidDeviceListChanged() { return false; }

  virtual bool OpenOutput(AudioOutputId output, std::string const& deviceName) {
    return false;
  }

  virtual void CloseOutput(AudioOutputId output) {}

  virtual bool IsOutputOpen(AudioOutputId output) { return output == AO_Main; }
};

}  // namespace Audio
}  // namespace Impacto