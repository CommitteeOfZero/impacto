#pragma once

#include <enum.h>
#include "../animation.h"

namespace Impacto {
namespace DelusionTrigger {

BETTER_ENUM(DelusionTriggerType, int, None, CHLCC)

class DelusionTriggerBase {
 public:
  enum DELUSION_STATE { NEUTRAL, POSITIVE, NEGATIVE };
  enum STATE { Hidden, Showing, Shown, Hiding };
  DelusionTriggerBase(int& delusionState, STATE showState);
  virtual void Update(float dt) = 0;
  virtual void Render() = 0;
  virtual void Show() = 0;
  virtual void Hide() = 0;
  virtual void Reset() = 0;

  Animation FadeAnimation;
  STATE State;
  int &DelusionState;
};

extern DelusionTriggerBase* Implementation;

void Init();
void Update(float dt);
void Show();
void Hide();
void Render();
void Reset();

}  // namespace DelusionTrigger
}  // namespace Impacto