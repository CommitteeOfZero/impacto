#include "game.h"
#include "profile.h"
#include "profile_internal.h"

#include <cstdint>

namespace Impacto {
namespace Profile {

void LoadGameFromLua() {
  AssertIs(LUA_TTABLE);

  ActiveRenderer = EnsureGetMember<RendererType>("ActiveRenderer");

  LayerCount = EnsureGetMember<int>("LayerCount");
  GameFeatures = EnsureGetMember<GameFeature>("GameFeatures");
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
  SubtitleConfig = TryGetMember<SubtitleConfigType>("SubtitleConfig")
                       .value_or(SubtitleConfigType::All);
  CloseBacklogWhenReachedEnd =
      TryGetMember<bool>("CloseBacklogWhenReachedEnd").value_or(true);
  DateFormat =
      TryGetMember<DateFormatType>("DateFormat").value_or(DateFormatType::YMD);
  HasScriptedExitLogic =
      TryGetMember<bool>("HasScriptedExitLogic").value_or(false);

  LayFileBigEndian = TryGetMember<bool>("LayFileBigEndian").value_or(false);
  CharaIsMvl = TryGetMember<bool>("CharaIsMvl").value_or(false);
  LayFileTexXMultiplier =
      TryGetMember<float>("LayFileTexXMultiplier").value_or(1.0f);
  LayFileTexYMultiplier =
      TryGetMember<float>("LayFileTexYMultiplier").value_or(1.0f);

  ScreenCaptureCount = TryGetMember<size_t>("ScreenCaptureCount").value_or(0);
  TryGetMember<bool>("UseMoviePriority", UseMoviePriority);
  TryGetMember<bool>("UseBgChaEffects", UseBgChaEffects);
  TryGetMember<bool>("UseBgFrameEffects", UseBgFrameEffects);
  TryGetMember<bool>("UseWaveEffects", UseWaveEffects);

  ActiveAudioBackend =
      TryGetMember<AudioBackendType>("AudioBackendType").value_or([] {
#ifndef IMPACTO_DISABLE_OPENAL
        return AudioBackendType::OpenAL;
#else
        return AudioBackendType::None;
#endif
      }());

  VideoPlayer = TryGetMember<VideoPlayerType>("VideoPlayerType").value_or([] {
#ifndef IMPACTO_DISABLE_FFMPEG
    return VideoPlayerType::FFmpeg;
#else
    return VideoPlayerType::None;
#endif
  }());

  TryGetMember<int>("PlatformId", PlatformId);
}

}  // namespace Profile
}  // namespace Impacto