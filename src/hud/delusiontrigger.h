#pragma once

#include <enum.h>
#include "../animation.h"

namespace Impacto {
namespace DelusionTrigger {

BETTER_ENUM(DelusionTriggerType, int, None, CHLCC, CCLCC)

class DelusionTriggerBase {
 public:
  enum DelusionState { DS_Neutral, DS_Positive, DS_Negative };
  enum UiState { Hidden, Showing, Shown, Hiding };
  DelusionTriggerBase(int32_t& delusionState, UiState showState);
  virtual void Update(float dt) = 0;
  virtual void Render() = 0;
  virtual void Hide() = 0;
  virtual void Show(){};
  virtual bool Show(int bgMtrgSelBufferId, int bgMtrgNegaPosiBufferId,
                    int param3) {
    return true;
  };
  virtual bool CheckTransitionAnimationComplete() { return true; };
  virtual bool CheckStartTransitionComplete() { return true; };

  UiState State;
  int32_t& DelusionState;
};

extern DelusionTriggerBase* Implementation;

void Init();
void Update(float dt);
void Show();
void Hide();
void Render();
bool Show(int bgMtrgSelBufferId, int bgMtrgNegaPosiBufferId, int param3);
bool CheckTransitionAnimationComplete();
bool CheckStartTransitionComplete();

}  // namespace DelusionTrigger
}  // namespace Impacto