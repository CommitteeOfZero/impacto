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
      child.ChildAnimation->Stop();
      child.ChildAnimation->Progress = 0;
    }

    float time = Progress * DurationIn;
    if (child.StartTime <= time && time <= child.EndTime) {
      child.ChildAnimation->StartIn();
    }
  }
}

void SequencedAnimation::StartOutImpl(bool reset) {
  for (ChildAnimation& child : Children) {
    if (reset) {
      child.ChildAnimation->Stop();
      child.ChildAnimation->Progress = 1;
    }

    float time = Progress * DurationOut;
    if (child.StartTime <= time && time <= child.EndTime) {
      child.ChildAnimation->StartOut();
    }
  }
}

void SequencedAnimation::FinishImpl() {
  for (ChildAnimation& child : Children) {
    child.ChildAnimation->Finish();
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
      if (child.ChildAnimation->State == +AnimationState::Playing) {
        child.ChildAnimation->Stop();
        child.ChildAnimation->Progress = time > child.StartTime;
      }

      continue;
    }

    if (Direction == +AnimationDirection::In) {
      float delta = time - previousTime;
      bool shouldStart = delta < 0 || (previousTime <= child.StartTime &&
                                       time >= child.StartTime);

      if (shouldStart) {
        child.ChildAnimation->StartIn(true);
        child.ChildAnimation->Update(time - child.StartTime);
        continue;
      }

      child.ChildAnimation->Update(delta);
    } else {
      float delta = previousTime - time;
      bool shouldStart =
          delta < 0 || (previousTime >= child.EndTime && time <= child.EndTime);

      if (shouldStart) {
        child.ChildAnimation->StartOut(true);
        child.ChildAnimation->Update(child.EndTime - time);
        continue;
      }

      child.ChildAnimation->Update(delta);
    }
  }
}

}  // namespace Impacto