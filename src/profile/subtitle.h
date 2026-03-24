#pragma once
#include <ankerl/unordered_dense.h>
#include <string>
#include <magic_enum/magic_enum.hpp>
#include "game.h"

namespace Impacto::Profile::Subtitle {

enum class SubtitleType : uint8_t {
  None = 0,
  Bitmap,
  Text,
  Ass,
};

struct SubtitleTrackFile {
  SubtitleType Type;
  SubtitleConfigType Config;
  std::optional<int> Id;
  std::optional<std::string> Path;
};

using SubtitleTrackFiles = std::vector<SubtitleTrackFile>;
inline ankerl::unordered_dense::map<std::string, SubtitleTrackFiles>
    SubtitleMappings;
inline std::vector<std::string> SubtitleFontsDir;

void Configure();
}  // namespace Impacto::Profile::Subtitle