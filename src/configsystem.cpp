#include "configsystem.h"

#include <algorithm>

namespace Impacto {
namespace ConfigSystem {

void Init() {
  std::fill_n(VoiceMuted, VoiceCount, false);
  std::fill_n(VoiceVolume, VoiceCount, 1.0f);
}

}  // namespace ConfigSystem
}  // namespace Impacto