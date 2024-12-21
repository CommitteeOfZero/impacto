#include "clock.h"

namespace Impacto::Video {

void Clock::SyncTo(Clock const& target) {
  std::optional<DoubleSeconds> clock = Get();
  std::optional<DoubleSeconds> targetClock = target.Get();
  if ((!targetClock && clock) ||
      std::chrono::abs(*clock - *targetClock) > DoubleSeconds(10.0)) {
    Set(*targetClock, target.Serial);
    Paused = false;
  }
}

void Clock::Set(av::Timestamp const& pts, int serial) {
  Paused = false;
  DoubleSeconds time = MonotonicClock::now().time_since_epoch();
  Pts = DoubleSeconds{pts.seconds()};
  LastUpdated = time;
  PtsDrift = Pts - time;
  Serial = serial;
}

std::optional<Clock::DoubleSeconds> Clock::Get() const {
  if (Pts == DoubleSeconds::zero()) {
    return std::nullopt;
  }
  if (Paused) {
    return Pts;
  }
  DoubleSeconds time = MonotonicClock::now().time_since_epoch();
  return PtsDrift + time - (time - LastUpdated) * (1.0 - Speed);
}

}  // namespace Impacto::Video