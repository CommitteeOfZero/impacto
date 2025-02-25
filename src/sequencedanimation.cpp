#include "sequencedanimation.h"

namespace Impacto {

void SequencedAnimation::AddAnimation(Animation* animation, float startTime,
                                      float duration) {
  float endTime = startTime + duration;
  Children.push_back({animation, startTime, endTime});

  // There are multiple conflicting ways to interpret playing a sequenced
  // animation in reverse, therefore doing so is currently undefined
  DurationIn = std::max(DurationIn, endTime);
  DurationOut = DurationIn;
}

void SequencedAnimation::StartInImpl(bool reset) {
  for (ChildAnimation& child : Children) {
    if (reset) {
      child.Animation->Stop();
      child.Animation->Progress = 0;
    }

    float time = Progress * DurationIn;
    if (child.StartTime <= time && time <= child.EndTime) {
      child.Animation->StartIn();
    }
  }
}

void SequencedAnimation::StartOutImpl(bool reset) {
  for (ChildAnimation& child : Children) {
    if (reset) {
      child.Animation->Stop();
      child.Animation->Progress = 1;
    }

    float time = Progress * DurationOut;
    if (child.StartTime <= time && time <= child.EndTime) {
      child.Animation->StartOut();
    }
  }
}

void SequencedAnimation::UpdateImpl(float dt) {
  float duration =
      Direction == +AnimationDirection::In ? DurationIn : DurationOut;
  float previousTime = Progress * duration;

  AddDelta(dt);

  duration = Direction == +AnimationDirection::In ? DurationIn : DurationOut;
  float time = Progress * duration;

  for (ChildAnimation& child : Children) {
    if (time < child.StartTime || child.EndTime < time) {
      if (child.Animation->State == +AnimationState::Playing) {
        child.Animation->Stop();
        child.Animation->Progress = time > child.StartTime;
      }

      continue;
    }

    if (Direction == +AnimationDirection::In) {
      float delta = time - previousTime;
      bool shouldStart = delta < 0 || (previousTime <= child.StartTime &&
                                       time >= child.StartTime);

      if (shouldStart) {
        child.Animation->StartIn(true);
        child.Animation->Update(time - child.StartTime);
        continue;
      }

      child.Animation->Update(delta);
    } else {
      float delta = previousTime - time;
      bool shouldStart =
          delta < 0 || (previousTime >= child.EndTime && time <= child.EndTime);

      if (shouldStart) {
        child.Animation->StartOut(true);
        child.Animation->Update(child.EndTime - time);
        continue;
      }

      child.Animation->Update(delta);
    }
  }
}

}  // namespace Impacto