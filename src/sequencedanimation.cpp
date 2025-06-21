#include "sequencedanimation.h"

namespace Impacto {

void SequencedAnimation::AddAnimation(Animation& animation, float startTime,
                                      float duration) {
  float endTime = startTime + duration;
  Children.push_back({std::ref(animation), startTime, endTime});

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
      childAnimation.Progress = 0;
    }

    float time = Progress * DurationIn;
    if (child.StartTime <= time && time <= child.EndTime) {
      childAnimation.StartIn();
    }
  }
}

void SequencedAnimation::StartOutImpl(bool reset) {
  for (ChildAnimation& child : Children) {
    Animation& childAnimation = child.ChildAnimation.get();

    if (reset) {
      childAnimation.Stop();
      childAnimation.Progress = 1;
    }

    float time = Progress * DurationOut;
    if (child.StartTime <= time && time <= child.EndTime) {
      childAnimation.StartOut();
    }
  }
}

void SequencedAnimation::FinishImpl() {
  for (ChildAnimation& child : Children) {
    child.ChildAnimation.get().Finish();
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
    Animation& childAnimation = child.ChildAnimation.get();

    if (time < child.StartTime || child.EndTime < time) {
      if (childAnimation.State == +AnimationState::Playing) {
        childAnimation.Stop();
        childAnimation.Progress = time > child.StartTime;
      }

      continue;
    }

    if (Direction == +AnimationDirection::In) {
      float delta = time - previousTime;
      bool shouldStart = delta < 0 || (previousTime <= child.StartTime &&
                                       time >= child.StartTime);

      if (shouldStart) {
        childAnimation.StartIn(true);
        childAnimation.Update(time - child.StartTime);
        continue;
      }

      childAnimation.Update(delta);
    } else {
      float delta = previousTime - time;
      bool shouldStart =
          delta < 0 || (previousTime >= child.EndTime && time <= child.EndTime);

      if (shouldStart) {
        childAnimation.StartOut(true);
        childAnimation.Update(child.EndTime - time);
        continue;
      }

      childAnimation.Update(delta);
    }
  }
}

}  // namespace Impacto