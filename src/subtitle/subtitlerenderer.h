#pragma once

#include <vector>
#include <libavcodec/avcodec.h>
#include <avcpp/timestamp.h>
#include <ankerl/unordered_dense.h>
#include <memory>
#include <concepts>
#include <variant>

#include "../video/clock.h"

namespace Impacto::Subtitle {
class SubtitlePlayer;

struct SubtitleEntry {
  struct BitmapData {
    int X{};
    int Y{};
    int W{};
    int H{};
    int NbColors{};
    std::array<std::unique_ptr<uint8_t[]>, 4> Data{};
    int LineSize[4]{};
  };

  std::variant<std::monostate, BitmapData, std::string> Data;
  int Flags{};
  std::chrono::milliseconds StartMs{};
  std::chrono::milliseconds Duration{};
};

class SubtitleRenderer {
 public:
  SubtitleRenderer(SubtitlePlayer& player, float width, float height)
      : Player(player), Width(width), Height(height) {}
  virtual ~SubtitleRenderer() noexcept = default;
  virtual void AddTrackFile(int trackId, std::span<const char> fileBuffer) = 0;
  virtual void AddSubtitleEntry(int trackId, SubtitleEntry entry) = 0;
  virtual void Update(Video::Clock::Microseconds elapsedTime) = 0;
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