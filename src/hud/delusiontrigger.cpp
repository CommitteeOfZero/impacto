#include "../profile/hud/delusiontrigger.h"

namespace Impacto {
namespace DelusionTrigger {

DelusionTriggerBase::DelusionTriggerBase(int& delusionState, UiState showState)
    : State(showState), DelusionState(delusionState){};

DelusionTriggerBase* Implementation = 0;

void Init() { Profile::DelusionTrigger::Configure(); }

void Show() {
  if (Implementation) Implementation->Show();
}

bool Show(int param1, int param2, int param3) {
  if (Implementation) return Implementation->Show(param1, param2, param3);
}

bool CheckTransitionAnimationComplete() {
  if (Implementation) return Implementation->CheckTransitionAnimationComplete();
}

bool CheckStartTransitionComplete() {
  if (Implementation) return Implementation->CheckStartTransitionComplete();
}

void Hide() {
  if (Implementation) Implementation->Hide();
}

void Update(float dt) {
  if (Implementation) Implementation->Update(dt);
}

void Render() {
  if (Implementation) Implementation->Render();
}

}  // namespace DelusionTrigger
}  // namespace Impacto