#include "waveeffects.h"

#include "../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace WaveEffects {
void Load() {
  EnsurePushMemberOfType("WaveEffects", LUA_TTABLE);
  BGWaveGridSize = EnsureGetMember<glm::u16vec2>("BGWaveGridSize");
  WaveMaxCount = EnsureGetMember<uint32_t>("WaveMaxCount");
  Pop();
}
}  // namespace WaveEffects
}  // namespace Profile
}  // namespace Impacto
