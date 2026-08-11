#pragma once

#include <string>
#include <ankerl/unordered_dense.h>
#include "profile/subtitle.h"

namespace Impacto::UserConfig {
struct GameConfig {
  std::string PatchProfile;
  bool UsePatch;
};
struct Config {
  int ResolutionWidth = 1280;
  int ResolutionHeight = 720;
  bool UseNativeGameResolution = false;
  bool Fullscreen = false;

  Profile::Subtitle::SubtitleConfigType SubtitleConfig =
      Profile::Subtitle::SubtitleConfigType::All;

  bool CloseBacklogWhenReachedEnd = true;
};

GameConfig& ActiveGameSettings();
void WriteUserConfig();

std::optional<std::string_view> GetPatchProfile();

inline ankerl::unordered_dense::map<std::string, GameConfig> GameSettings;
inline Config CommonSettings;
inline std::string ActiveGame;
inline std::string PatchProfileOverride;
inline std::string UserConfigPath;

void Configure();
}  // namespace Impacto::UserConfig