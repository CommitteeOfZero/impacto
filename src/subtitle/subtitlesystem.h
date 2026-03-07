#pragma once

#include <array>
#include <memory>
#include <vector>
#include <variant>
#include <string>

#include "subtitlerenderer.h"
#include "subtitlecommon.h"
#include "../video/clock.h"
#include "../profile/subtitle.h"

namespace Impacto::Subtitle {

class SubtitlePlayer {
 public:
  SubtitlePlayer(float width, float height);
  void AddTrack(int trackId, Profile::Subtitle::SubtitleType type,
                void* trackInfo);
  void AddTrackFile(int trackId, Profile::Subtitle::SubtitleType type,
                    std::string const& path);
  void PushEntry(int trackId, SubtitleEntry entry);
  void Update(Video::Clock::Seconds frameTime);
  void Render();
  int8_t GetBackendIndex(Profile::Subtitle::SubtitleType type);

 protected:
  std::array<std::unique_ptr<SubtitleRenderer>, 3> Backends;
  ankerl::unordered_dense::map<int, Profile::Subtitle::SubtitleType>
      TrackTypeMap;
};

void SubtitleInit();
}  // namespace Impacto::Subtitle