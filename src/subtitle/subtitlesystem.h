#pragma once

#include <array>
#include <memory>
#include <vector>
#include <variant>
#include <string>
#include <utility>

#include "subtitlerenderer.h"
#include "../video/clock.h"
#include "../profile/subtitle.h"

namespace Impacto::Subtitle {

template <typename Track, typename... TrackArgs>
concept HasAddTrack = requires {
  typename Track::BackendType;
  requires std::is_base_of_v<SubtitleRenderer, typename Track::BackendType>;

  {
    std::declval<typename Track::BackendType&>().AddTrack(
        std::declval<int>(), Track(std::declval<typename Track::BackendType&>(),
                                   std::declval<TrackArgs>()...))
  } -> std::same_as<void>;
};
class SubtitlePlayer {
 public:
  SubtitlePlayer(float width, float height);

  template <typename Track, typename... TrackArgs>
    requires HasAddTrack<Track, TrackArgs...>
  void AddTrack(int trackId, Profile::SubtitleConfigType config,
                TrackArgs&&... args);
  void AddTrackFile(int trackId, Profile::Subtitle::SubtitleType type,
                    std::string const& path,
                    Profile::SubtitleConfigType config);
  void PushEntry(int trackId, SubtitleEntry entry);
  void Update(Video::Clock::Microseconds elapsedTime);
  void Render();

 protected:
  int8_t GetBackendIndex(Profile::Subtitle::SubtitleType type) const;
  bool CanAddTrack(int trackId, Profile::Subtitle::SubtitleType type,
                   Profile::SubtitleConfigType config) const;

  std::array<std::unique_ptr<SubtitleRenderer>, 3> Backends;
  ankerl::unordered_dense::map<int, Profile::Subtitle::SubtitleType>
      TrackTypeMap;
};

void SubtitleInit();

template <typename Track, typename... TrackArgs>
  requires HasAddTrack<Track, TrackArgs...>
inline void SubtitlePlayer::AddTrack(int trackId,
                                     Profile::SubtitleConfigType config,
                                     TrackArgs&&... args) {
  constexpr auto type = Track::BackendType::Type;
  if (!CanAddTrack(trackId, type, config)) return;
  auto* backend =
      dynamic_cast<Track::BackendType*>(Backends[GetBackendIndex(type)].get());
  if (!backend) {
    throw std::runtime_error("Invalid subtitle track type");
  }
  backend->AddTrack(trackId, Track(*backend, std::forward<TrackArgs>(args)...));
  TrackTypeMap.try_emplace(trackId, type);
}
}  // namespace Impacto::Subtitle