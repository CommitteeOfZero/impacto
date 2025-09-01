#include "sequencedanimation.h"

namespace Impacto {

void SequencedAnimation::AddAnimation(Animation& animation, float startTime,
                                      float duration,
                                      AnimationDirection direction) {
  float endTime = startTime + duration;
  Children.push_back({std::ref(animation), startTime, endTime, direction});

  // There are multiple conflicting ways to interpret playing a sequenced
  // animation in reverse, therefore doing so is currently undefined
  DurationIn = std::max(DurationIn, endTime);
  DurationOut = DurationIn;
}

void SequencedAnimation::StartInImpl(bool reset) {
  for (ChildAnimation& child : Children) {
    Animation& childAnimation = child.ChildAnimation.get();
    if (reset) {
      childAnimation.Stop();
      childAnimation.Progress =
          child.Direction == AnimationDirection::In ? 0.0f : 1.0f;
    }

    float time = Progress * DurationIn;
    if (child.StartTime <= time && time <= child.EndTime) {
      childAnimation.Start(child.Direction);
    }
  }
}

void SequencedAnimation::StartOutImpl(bool reset) {
  for (ChildAnimation& child : Children) {
    Animation& childAnimation = child.ChildAnimation.get();

    if (reset) {
      childAnimation.Stop();
      childAnimation.Progress =
          child.Direction == AnimationDirection::In ? 1.0f : 0.0f;
    }

    float time = Progress * DurationOut;
    if (child.StartTime <= time && time <= child.EndTime) {
      childAnimation.Start(-child.Direction);
    }
  }
}

void SequencedAnimation::FinishImpl() {
  for (ChildAnimation& child : Children) {
    child.ChildAnimation.get().Finish();
  }
}

void SequencedAnimation::UpdateImpl(float dt) {
  float duration = GetDuration(Direction);
  float previousTime = Progress * duration;

  AddDelta(dt);

  duration = GetDuration(Direction);
  float time = Progress * duration;

  for (ChildAnimation& child : Children) {
    Animation& childAnimation = child.ChildAnimation.get();

    if (time < child.StartTime || child.EndTime < time) {
      if (childAnimation.State == +AnimationState::Playing) {
        childAnimation.Stop();
        childAnimation.Progress = time > child.StartTime;

        if (childAnimation.Direction == AnimationDirection::Out) {
          childAnimation.Progress = 1.0f - childAnimation.Progress;
        }
      }

      continue;
    }

    if (Direction == AnimationDirection::In) {
      float delta = time - previousTime;
      bool shouldStart = delta < 0 || (previousTime <= child.StartTime &&
                                       time >= child.StartTime);

      if (shouldStart) {
        childAnimation.Start(child.Direction, true);
        childAnimation.Update(time - child.StartTime);
        continue;
      }

      childAnimation.Update(delta);
    } else {
      float delta = previousTime - time;
      bool shouldStart =
          delta < 0 || (previousTime >= child.EndTime && time <= child.EndTime);

      if (shouldStart) {
        childAnimation.Start(-child.Direction, true);
        childAnimation.Update(child.EndTime - time);
        continue;
      }

      childAnimation.Update(delta);
    }
  }
}

}  // namespace Impacto