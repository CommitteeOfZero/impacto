#include "animation.h"
#include <vector>

namespace Impacto {

struct ChildAnimation {
  std::reference_wrapper<Animation> ChildAnimation;
  float StartTime;
  float EndTime;
  AnimationDirection Direction;
};

class SequencedAnimation : public Animation {
 public:
  void AddAnimation(Animation& animation, float startTime, float duration,
                    AnimationDirection direction = AnimationDirection::In);

  void AddAnimation(Animation& animation, float startTime,
                    AnimationDirection direction = AnimationDirection::In) {
    AddAnimation(animation, startTime, animation.GetDuration(direction),
                 direction);
  }

  void AddAnimation(Animation& animation,
                    AnimationDirection direction = AnimationDirection::In) {
    AddAnimation(animation, DurationIn, direction);
  }

 protected:
  void UpdateImpl(float dt) override;
  void StartInImpl(bool reset) override;
  void StartOutImpl(bool reset) override;
  void ResetImpl(std::optional<AnimationDirection> direction) override;
  void FinishImpl() override;

 private:
  std::vector<ChildAnimation> Children;
};

}  // namespace Impacto