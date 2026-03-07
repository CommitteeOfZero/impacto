#include "clock.h"
#include "../impacto.h"

namespace Impacto::Video {

using namespace std::chrono;
Clock::Clock()
    : Pts(NAN), LastUpdated(Now()), Speed(1.0), Serial(-1), Paused(false) {}

void Clock::SyncTo(Clock* target) {
  Seconds clock = Get();
  Seconds targetClock = target->Get();
  if (!isnan(targetClock.count()) &&
      (isnan(clock.count()) || abs(clock - targetClock) > 10.0s))
    Set(targetClock, target->Serial);
}

void Clock::Set(Seconds pts, int serial) {
  Pts = pts;
  LastUpdated = std::chrono::steady_clock::now();
  Serial = serial;
}

Clock::Seconds Clock::Get() {
  if (Paused) {
    return Pts;
  } else {
    auto elapsed = Now() - LastUpdated;
    return Pts + Seconds(elapsed) * Speed;
  }
}

}  // namespace Impacto::Video