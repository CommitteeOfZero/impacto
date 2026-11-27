#pragma once

#include <string>
#include <ankerl/unordered_dense.h>
#include "renderer/window.h"
#include "profile/subtitle.h"
#include "log.h"

#ifndef IMPACTO_DISABLE_IMGUI
#include "overlay/notification/toastnotification.h"
#endif

namespace Impacto {

enum class RendererType : int {
  None,
#ifndef IMPACTO_DISABLE_OPENGL
  OpenGL,
#endif
#ifndef IMPACTO_DISABLE_VULKAN
  Vulkan,
#endif
#ifndef IMPACTO_DISABLE_DX9
  DirectX9,
#endif
};

enum class VideoPlayerType : int {
  None,
  FFmpeg,
};

enum class AudioBackendType : int {
  None,
#ifndef IMPACTO_DISABLE_OPENAL
  OpenAL,
#endif
};

enum class SubtitleAssBackendType : int {
  None,
#ifndef IMPACTO_DISABLE_LIBASS
  LibAss,
#endif
};

enum class SubtitleTextBackendType : int {
  None,
};

enum class SubtitleBmpBackendType : int {
  None,
};

namespace UserConfig {
struct GameConfig {
  std::optional<int> ResolutionWidth;
  std::optional<int> ResolutionHeight;
  std::string PatchProfile;
  bool UsePatch;
  bool UseVSync = true;
  DisplayMode Display = GetDefaultDispMode();
};
struct Config {
  int ResolutionWidth = 1280;
  int ResolutionHeight = 720;

#ifndef IMPACTO_DISABLE_IMGUI
  Overlay::NotificationType EnabledNotifications =
      Overlay::NotificationType::All;
#endif

  std::string LogFile = GetDefaultLogFile();
  LogLevel LogLvl = LogLevel::Warning;
  LogChannel LogChannels = LogChannel::All;
  bool LoggingToConsole = GetDefaultLogToConsole();
  bool LoggingToFile = GetDefaultLogToFile();
  bool SwapConfirmCancelButtons = false;
};
struct AdvancedConfig {
  RendererType ActiveRenderer = RendererType::OpenGL;
  VideoPlayerType VideoPlayer = VideoPlayerType::FFmpeg;
  AudioBackendType ActiveAudioBackend = AudioBackendType::OpenAL;
  SubtitleAssBackendType SubtitleAssBackend = SubtitleAssBackendType::LibAss;
  SubtitleTextBackendType SubtitleTextBackend = SubtitleTextBackendType::None;
  SubtitleBmpBackendType SubtitleBmpBackend = SubtitleBmpBackendType::None;
  bool ForceBorderless = false;
};

struct CHLCCEnhancements {
  bool DelusionMousePatch = true;
};

struct CCLCCEnhancements {
  bool DelusionMousePatch = true;
};

struct EnhancementsConfig {
  Profile::Subtitle::SubtitleConfigType SubtitleConfig =
      Profile::Subtitle::SubtitleConfigType::All;

  bool CloseBacklogWhenReachedEnd = true;

  CHLCCEnhancements CHLCC;
  CCLCCEnhancements CCLCC;
};

GameConfig& ActiveGameSettings();
bool GetEffectiveVSync();
void WriteUserConfig();
void RestoreSettingsDefaults();

std::optional<std::string_view> GetPatchProfile();
std::string const& GetActiveGame();
void SetActiveGame(std::string activeGame);

inline ankerl::unordered_dense::map<std::string, GameConfig> GameSettings;
inline Config CommonSettings;
inline AdvancedConfig AdvancedSettings;
inline EnhancementsConfig EnhancementsSettings;

inline std::string PatchProfileOverride;
inline std::string UserConfigPath;

inline bool OverrideLogChannels;

void Configure();
}  // namespace UserConfig
}  // namespace Impacto
