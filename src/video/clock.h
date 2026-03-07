#pragma once
#include <chrono>
namespace Impacto::Video {

class Clock {
 public:
  using Seconds = std::chrono::duration<double>;
  using MonotonicTime = std::chrono::steady_clock::time_point;
  Seconds Pts;
  MonotonicTime LastUpdated;
  float Speed;
  int Serial;
  bool Paused;

  Clock();
  void SyncTo(Clock* target);
  void Set(Seconds pts, int serial);
  Seconds Get();
  static MonotonicTime Now() { return std::chrono::steady_clock::now(); };
};
}  // namespace Impacto::Video