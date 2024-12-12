#include "clock.h"

namespace Impacto::Video {

Clock::Clock() {
  Speed = 1.0;
  Paused = true;
  Pts = MonotonicClock::duration::min();
  LastUpdated = MonotonicClock::duration::min();
  PtsDrift = MonotonicClock::duration::min();
  Serial = -1;
}

void Clock::SyncTo(Clock const& target) {
  DoubleSeconds clock = Get();
  DoubleSeconds targetClock = target.Get();
  if (targetClock == MonotonicClock::duration::min() &&
      (clock != MonotonicClock::duration::min() ||
       std::chrono::abs(clock - targetClock) > DoubleSeconds(10.0))) {
    Set(targetClock, target.Serial);
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

Clock::DoubleSeconds Clock::Get() const {
  if (Paused || Pts == MonotonicClock::duration::min()) {
    return Pts;
  }
  DoubleSeconds time = MonotonicClock::now().time_since_epoch();
  return PtsDrift + time - (time - LastUpdated) * (1.0 - Speed);
}

}  // namespace Impacto::Video