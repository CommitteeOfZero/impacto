#include "game.h"
#include "profile.h"
#include "profile_internal.h"

#include <cstdint>

namespace Impacto {
namespace Profile {

namespace Game {
void Configure() {
  AssertIs(LUA_TTABLE);
  LayerCount = EnsureGetMember<int>("LayerCount");
  GameFeatures = EnsureGetMember<GameFeature>("GameFeatures");
  WindowName = EnsureGetMember<std::string>("WindowName");
  WindowIconPath = TryGetMember<std::string>("WindowIconPath");
  CursorArrowPath = TryGetMember<std::string>("CursorArrowPath");
  CursorPointerPath = TryGetMember<std::string>("CursorPointerPath");
  DesignWidth = EnsureGetMember<float>("DesignWidth");
  DesignHeight = EnsureGetMember<float>("DesignHeight");

  LayFileBigEndian = TryGetMember<bool>("LayFileBigEndian").value_or(false);
  CharaIsMvl = TryGetMember<bool>("CharaIsMvl").value_or(false);
  LayFileTexXMultiplier =
      TryGetMember<float>("LayFileTexXMultiplier").value_or(1.0f);
  LayFileTexYMultiplier =
      TryGetMember<float>("LayFileTexYMultiplier").value_or(1.0f);

  ScreenCaptureCount = TryGetMember<size_t>("ScreenCaptureCount").value_or(0);
  TryGetMember<bool>("UseMoviePriority", UseMoviePriority);
  TryGetMember<bool>("UseWaveEffects", UseWaveEffects);

  TryGetMember<int>("PlatformId", PlatformId);

  HasInit = true;
}
}  // namespace Game
}  // namespace Profile
}  // namespace Impacto