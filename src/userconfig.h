#pragma once

#include <string>
#include <ankerl/unordered_dense.h>
#include "renderer/window.h"
#include "profile/subtitle.h"
#include "log.h"

namespace Impacto {

enum class RendererType : int {
  OpenGL,
  Vulkan,
  DirectX9,
};

enum class VideoPlayerType : int {
  None,
  FFmpeg,
};

enum class AudioBackendType : int {
  None,
  OpenAL,
};

enum class SubtitleAssBackendType : int {
  None,
  LibAss,
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
  DisplayMode Display = GetDefaultDispMode();
};
struct Config {
  int ResolutionWidth = 1280;
  int ResolutionHeight = 720;

  std::string LogFile = "Impacto_Log.txt";
  LogLevel LogLvl = LogLevel::Error;
  LogChannel LogChannels = LogChannel::All;
  bool LoggingToConsole = true;
  bool LoggingToFile = true;
};
struct AdvancedConfig {
  RendererType ActiveRenderer = RendererType::OpenGL;
  VideoPlayerType VideoPlayer = VideoPlayerType::FFmpeg;
  AudioBackendType ActiveAudioBackend = AudioBackendType::OpenAL;
  SubtitleAssBackendType SubtitleAssBackend = SubtitleAssBackendType::LibAss;
  SubtitleTextBackendType SubtitleTextBackend = SubtitleTextBackendType::None;
  SubtitleBmpBackendType SubtitleBmpBackend = SubtitleBmpBackendType::None;
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
