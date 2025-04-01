#include "animation.h"

namespace Impacto {

void Animation::AddDelta(float dt) {
  float duration =
      Direction == +AnimationDirection::In ? DurationIn : DurationOut;

  switch (LoopMode) {
    case AnimationLoopMode::Stop: {
      float endProgress = Direction == +AnimationDirection::In ? 1.0f : 0.0f;

      Progress = std::clamp(
          Progress + static_cast<float>(Direction) * dt / duration, 0.0f, 1.0f);
      if (Progress == endProgress) State = AnimationState::Stopped;
      return;
    }
    case AnimationLoopMode::Loop: {
      // E.g. Progress = 1.04 => Progress = 0.04
      Progress += static_cast<float>(Direction) * dt / duration;
      Progress -= std::floor(Progress);
      return;
    }
    case AnimationLoopMode::ReverseDirection: {
      // E.g. Progress = 1.04 => Progress = 0.96
      float cycleDuration = DurationIn + DurationOut;

      float time = Progress * duration;
      if (Direction == +AnimationDirection::Out) {
        time = cycleDuration - time;
      }

      time = std::fmod(time + dt, cycleDuration);

      if (time < DurationIn) {
        Progress = time / DurationIn;
        Direction = AnimationDirection::In;
      } else {
        Progress = 1.0f - (time - DurationIn) / DurationOut;
        Direction = AnimationDirection::Out;
      }

      return;
    }
  }
}

}  // namespace Impacto