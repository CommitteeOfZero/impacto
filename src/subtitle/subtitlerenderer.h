#pragma once

#include <vector>
#include <libavcodec/avcodec.h>
#include <avcpp/timestamp.h>
#include <ankerl/unordered_dense.h>
#include <memory>
#include <concepts>

#include "subtitlecommon.h"
#include "../video/clock.h"

namespace Impacto::Subtitle {
class SubtitlePlayer;
class SubtitleRenderer {
 public:
  SubtitleRenderer(SubtitlePlayer& player, float width, float height)
      : Player(player), Width(width), Height(height) {}
  virtual ~SubtitleRenderer() noexcept = default;
  virtual void AddTrack(int trackId, void* trackInfo) = 0;
  virtual void AddTrackFile(int trackId, std::span<const char> fileBuffer) = 0;
  virtual void AddSubtitleEntry(int trackId, SubtitleEntry entry) = 0;
  virtual void Update(Video::Clock::Seconds frameTime) = 0;
  virtual void Render() = 0;
  virtual void SetTrackVisibility(int trackId, bool enable) = 0;
  float GetWidth() const { return Width; }
  float GetHeight() const { return Height; }

 protected:
  std::reference_wrapper<SubtitlePlayer> Player;
  float Width;
  float Height;
};

}  // namespace Impacto::Subtitle