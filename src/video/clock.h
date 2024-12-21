#pragma once

#include <chrono>
#include <optional>
#include <timestamp.h>

namespace Impacto::Video {

class Clock {
 public:
  using MonotonicClock = std::chrono::steady_clock;
  using DoubleSeconds = std::chrono::duration<double>;
  DoubleSeconds Pts{};
  // duration between monotonic clock and frame timestamp
  DoubleSeconds PtsDrift{};
  DoubleSeconds LastUpdated{};
  double Speed = 1.0;
  int Serial = -1;
  bool Paused = true;
  bool Init = false;

  void SyncTo(Clock const& target);
  void Set(av::Timestamp const& pts, int serial);
  std::optional<DoubleSeconds> Get() const;
};

}  // namespace Impacto::Video