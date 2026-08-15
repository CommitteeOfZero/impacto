#pragma once

#include <string>
#include <ankerl/unordered_dense.h>
#include "profile/subtitle.h"

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

struct CHLCCExtraConfig {
  bool DelusionMousePatch = true;
};

struct CCLCCExtraConfig {
  bool DelusionMousePatch = true;
};

using GameExtraConfig =
    std::variant<std::monostate, CHLCCExtraConfig, CCLCCExtraConfig>;
struct GameConfig {
  std::optional<int> ResolutionWidth;
  std::optional<int> ResolutionHeight;
  std::string PatchProfile;
  bool UsePatch;
  bool Fullscreen = false;

  GameExtraConfig Extra;
};
struct Config {
  int ResolutionWidth = 1280;
  int ResolutionHeight = 720;

  Profile::Subtitle::SubtitleConfigType SubtitleConfig =
      Profile::Subtitle::SubtitleConfigType::All;

  bool CloseBacklogWhenReachedEnd = true;
};
struct AdvancedConfig {
  RendererType ActiveRenderer = RendererType::OpenGL;
  VideoPlayerType VideoPlayer = VideoPlayerType::FFmpeg;
  AudioBackendType ActiveAudioBackend = AudioBackendType::OpenAL;
  SubtitleAssBackendType SubtitleAssBackend = SubtitleAssBackendType::LibAss;
  SubtitleTextBackendType SubtitleTextBackend = SubtitleTextBackendType::None;
  SubtitleBmpBackendType SubtitleBmpBackend = SubtitleBmpBackendType::None;
};

GameConfig& ActiveGameSettings();
void WriteUserConfig();

std::optional<std::string_view> GetPatchProfile();
std::string const& GetActiveGame();
void SetActiveGame(std::string activeGame);

inline ankerl::unordered_dense::map<std::string, GameConfig> GameSettings;
inline Config CommonSettings;
inline AdvancedConfig AdvancedSettings;

inline std::string ActiveGameOverride;
inline std::string PatchProfileOverride;
inline std::string UserConfigPath;

void Configure();
}  // namespace UserConfig
}  // namespace Impacto
