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
  WindowIconPath = TryGetMember<std::string>("WindowIconPath");
  CursorArrowPath = TryGetMember<std::string>("CursorArrowPath");
  CursorPointerPath = TryGetMember<std::string>("CursorPointerPath");
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
  TryGetMember<bool>("UseBgChaEffects", UseBgChaEffects);
  TryGetMember<bool>("UseBgFrameEffects", UseBgFrameEffects);

  int audioBackendType = -1;
  res = TryGetMember<int>("AudioBackendType", audioBackendType);
  if (!res) {
#ifndef IMPACTO_DISABLE_OPENAL
    ActiveAudioBackend = AudioBackendType::OpenAL;
#else
    ActiveAudioBackend = AudioBackendType::None;
#endif
  } else
    ActiveAudioBackend =
        AudioBackendType::_from_integral_unchecked(audioBackendType);
  int videoPlayerType = -1;
  res = TryGetMember<int>("VideoPlayerType", videoPlayerType);
  if (!res) {
#ifndef IMPACTO_DISABLE_FFMPEG
    VideoPlayer = VideoPlayerType::FFmpeg;
#else
    VideoPlayer = VideoPlayerType::None;
#endif
  } else
    VideoPlayer = VideoPlayerType::_from_integral_unchecked(videoPlayerType);

  TryGetMember<int>("PlatformId", PlatformId);
}

}  // namespace Profile
}  // namespace Impacto