#pragma once
#include <ankerl/unordered_dense.h>
#include <string>
#include <enum.h>

namespace Impacto::Profile::Subtitle {

BETTER_ENUM(SubtitleType, uint8_t, NONE = 0, BITMAP, TEXT, ASS)

using SubtitleTrackFiles = std::vector<std::pair<SubtitleType, std::string>>;
inline ankerl::unordered_dense::map<std::string, SubtitleTrackFiles>
    SubtitleMappings;
inline std::optional<std::string> SubtitleFontsDir;

void Configure();
}  // namespace Impacto::Profile::Subtitle