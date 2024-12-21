#include "clock.h"

extern "C" {
#include <libavutil/time.h>
}

namespace Impacto::Video {

Clock::Clock()
    : Pts(NAN),
      PtsDrift(NAN),
      LastUpdated(av_gettime_relative() / 1000000.0),
      Speed(1.0),
      Serial(-1),
      Paused(false) {}

void Clock::SyncTo(Clock* target) {
  double clock = Get();
  double targetClock = target->Get();
  if (!isnan(targetClock) && (isnan(clock) || fabs(clock - targetClock) > 10.0))
    Set(targetClock, target->Serial);
}

void Clock::Set(double pts, int serial) {
  double time = av_gettime_relative() / 1000000.0;
  Pts = pts;
  LastUpdated = time;
  PtsDrift = Pts - time;
  Serial = serial;
}

double Clock::Get() {
  if (Paused) {
    return Pts;
  } else {
    double time = av_gettime_relative() / 1000000.0;
    return PtsDrift + time - (time - LastUpdated) * (1.0 - Speed);
  }
}

}  // namespace Impacto::Video