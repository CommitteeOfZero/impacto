#pragma once

#include <string>
#include <ankerl/unordered_dense.h>
#include "subtitle.h"

namespace Impacto::Profile::UserConfig {
struct GameConfig {
  std::string ActivePatch;
};
struct Config {
  int ResolutionWidth;
  int ResolutionHeight;
  bool UseNativeGameResolution;
  bool Fullscreen;

  Subtitle::SubtitleConfigType SubtitleConfig =
      Subtitle::SubtitleConfigType::None;

  bool CloseBacklogWhenReachedEnd = true;
};

GameConfig const& ActiveGameSettings();

inline ankerl::unordered_dense::map<std::string, GameConfig> GameSettings;
inline Config CommonSettings;
inline std::string ActiveGame;
inline std::string PatchOverride;

void Configure();
std::string const& GetPlatformSpecificPath();
}  // namespace Impacto::Profile::UserConfig