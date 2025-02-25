#include "animation.h"
#include <vector>

namespace Impacto {

struct ChildAnimation {
  Animation* Animation;
  float StartTime;
  float EndTime;
};

class SequencedAnimation : public Animation {
 public:
  void AddAnimation(Animation* animation, float startTime, float duration);

  void AddAnimation(Animation* animation, float startTime) {
    AddAnimation(animation, startTime, animation->DurationIn);
  }

  void AddAnimation(Animation* animation) {
    AddAnimation(animation, DurationIn);
  }

 protected:
  void UpdateImpl(float dt) override;
  void StartInImpl(bool reset) override;
  void StartOutImpl(bool reset) override;

 private:
  std::vector<ChildAnimation> Children;
};

}  // namespace Impacto