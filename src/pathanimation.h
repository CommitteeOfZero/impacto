#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include "enum.h"
#include "animation.h"

namespace Impacto {

BETTER_ENUM(EasingFunction, int, Linear, QuadraticIn, QuadraticOut, CubicIn,
            CubicOut)

float Ease(float progress, EasingFunction function = EasingFunction::Linear);

struct PathSegment {
  glm::vec2 StartPosition;
  glm::vec2 EndPosition;
  float Duration = 0;
  EasingFunction EasingX = EasingFunction::Linear;
  EasingFunction EasingY = EasingX;
};

class PathAnimation : public Animation {
 public:
  PathAnimation() : PathAnimation(std::vector<PathSegment>()) {};
  PathAnimation(std::vector<PathSegment> segments);

  std::vector<PathSegment> Path;

  glm::vec2 GetPosition() const;
  size_t GetCurrentSegmentIndex() const;
  float GetCurrentSegmentProgress() const;

 protected:
  void UpdateImpl(float dt) override;

 private:
  size_t CurrentSegmentIndex = 0;
  float CurrentSegmentProgress = 0;
};

}  // namespace Impacto