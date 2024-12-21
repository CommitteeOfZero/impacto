#pragma once

#include <chrono>
#include <optional>

namespace Impacto::Video {

class Clock {
 public:
  double Pts;
  double PtsDrift;
  double LastUpdated;
  double Speed;
  int Serial;
  bool Paused;

  Clock();
  void SyncTo(Clock* target);
  void Set(double pts, int serial);
  double Get();
};
}  // namespace Impacto::Video