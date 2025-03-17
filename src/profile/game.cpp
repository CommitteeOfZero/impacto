#include "game.h"
#include "profile.h"
#include "profile_internal.h"

#include <cstdint>

namespace Impacto {
namespace Profile {

void LoadGameFromLua() {
  AssertIs(LUA_TTABLE);

  ActiveRenderer = RendererType::_from_integral_unchecked(
      EnsureGetMember<int>("ActiveRenderer"));

  LayerCount = EnsureGetMember<int>("LayerCount");
  GameFeatures = EnsureGetMember<int>("GameFeatures");
  WindowName = EnsureGetMember<char const*>("WindowName");
  DesignWidth = EnsureGetMember<float>("DesignWidth");
  DesignHeight = EnsureGetMember<float>("DesignHeight");

  Language = EnsureGetMember<char const*>("Language");
  ResolutionWidth = EnsureGetMember<int>("ResolutionWidth");
  ResolutionHeight = EnsureGetMember<int>("ResolutionHeight");
  Fullscreen = EnsureGetMember<bool>("Fullscreen");
  Subtitles = EnsureGetMember<char const*>("Subtitles");

  bool res = TryGetMember<bool>("LayFileBigEndian", LayFileBigEndian);
  if (!res) LayFileBigEndian = false;
  res = TryGetMember<bool>("CharaIsMvl", CharaIsMvl);
  if (!res) CharaIsMvl = false;
  res = TryGetMember<float>("LayFileTexXMultiplier", LayFileTexXMultiplier);
  if (!res) LayFileTexXMultiplier = 1.0f;
  res = TryGetMember<float>("LayFileTexYMultiplier", LayFileTexYMultiplier);
  if (!res) LayFileTexYMultiplier = 1.0f;
  res = TryGetMember<bool>("UseScreenCapEffects", UseScreenCapEffects);
  if (!res) UseScreenCapEffects = true;
  TryGetMember<bool>("UseMoviePriority", UseMoviePriority);
  int audioBackendType = -1;
  res = TryGetMember<int>("AudioBackendType", audioBackendType);
  if (!res)
    ActiveAudioBackend = AudioBackendType::OpenAL;
  else
    ActiveAudioBackend =
        AudioBackendType::_from_integral_unchecked(audioBackendType);
  int videoPlayerType = -1;
  res = TryGetMember<int>("VideoPlayerType", videoPlayerType);
  if (!res)
    VideoPlayer = VideoPlayerType::FFmpeg;
  else
    VideoPlayer = VideoPlayerType::_from_integral_unchecked(videoPlayerType);

  TryGetMember<int>("PlatformId", PlatformId);
}

}  // namespace Profile
}  // namespace Impacto