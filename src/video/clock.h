#pragma once

#include <chrono>
#include <timestamp.h>

namespace Impacto::Video {

class Clock {
 public:
  using MonotonicClock = std::chrono::steady_clock;
  using DoubleSeconds = std::chrono::duration<double>;
  DoubleSeconds Pts;
  // duration between monotonic clock and frame timestamp
  DoubleSeconds PtsDrift;
  DoubleSeconds LastUpdated;
  double Speed;
  int Serial;
  bool Paused;

  Clock();
  void SyncTo(Clock const& target);
  void Set(av::Timestamp const& pts, int serial);
  DoubleSeconds Get() const;
};

}  // namespace Impacto::Video