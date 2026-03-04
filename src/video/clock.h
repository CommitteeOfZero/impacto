#pragma once
#include <chrono>
namespace Impacto::Video {

class Clock {
 public:
  using Microseconds = std::chrono::microseconds;
  using MonotonicTime = std::chrono::steady_clock::time_point;
  Microseconds Pts;
  MonotonicTime LastUpdated;
  float Speed;
  int Serial;
  bool Paused;

  Clock();
  void SyncTo(Clock* target);
  void Set(Microseconds pts, int serial);
  Microseconds Get();
  static MonotonicTime Now() { return std::chrono::steady_clock::now(); };
};
}  // namespace Impacto::Video