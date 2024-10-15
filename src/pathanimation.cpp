#include "pathanimation.h"
#include <numeric>

namespace Impacto {

float Ease(float progress, EasingFunction function) {
  switch (function) {
    case EasingFunction::Linear: {
      return progress;
    }
    case EasingFunction::QuadraticIn: {
      return progress * progress;
    }
    case EasingFunction::QuadraticOut: {
      return 1 - (1 - progress) * (1 - progress);
    }
    case EasingFunction::CubicIn: {
      return progress * progress * progress;
    }
    case EasingFunction::CubicOut: {
      return 1 - std::pow(1 - progress, 3);
    }
  }
}

PathAnimation::PathAnimation(std::vector<PathSegment> path) : Path(path) {
  float totalDuration = 0;
  for (const PathSegment& segment : path) {
    totalDuration += segment.Duration;
  }

  this->DurationIn = totalDuration;
  this->DurationOut = totalDuration;
}

float PathAnimation::GetCurrentSegmentProgress() const {
  return CurrentSegmentProgress;
}

size_t PathAnimation::GetCurrentSegmentIndex() const {
  return CurrentSegmentIndex;
}

glm::vec2 PathAnimation::GetPosition() const {
  const PathSegment& segment = Path[CurrentSegmentIndex];
  glm::vec2 direction = segment.EndPosition - segment.StartPosition;

  float progress = GetCurrentSegmentProgress();
  return segment.StartPosition + glm::vec2(Ease(progress, segment.EasingX),
                                           Ease(progress, segment.EasingY)) *
                                     direction;
}

void PathAnimation::UpdateImpl(float dt) {
  Animation::UpdateImpl(dt);

  float segmentEndProgress = Direction == 1 ? 0 : 1;
  size_t segment = Direction == 1 ? 0 : Path.size() - 1;
  size_t end = Direction == 1 ? Path.size() : -1;

  while (segment != end) {
    float segmentStartProgress = segmentEndProgress;
    segmentEndProgress += Path[segment].Duration / this->DurationIn * Direction;

    if ((Direction == 1 && this->Progress < segmentEndProgress) ||
        (Direction == -1 && this->Progress > segmentEndProgress)) {
      CurrentSegmentIndex = segment;

      float segmentDuration = Path[segment].Duration;
      CurrentSegmentProgress = (this->Progress - std::min(segmentStartProgress,
                                                          segmentEndProgress)) /
                               (segmentDuration / this->DurationIn);

      break;
    }

    segment += Direction;
  }
}

}  // namespace Impacto
