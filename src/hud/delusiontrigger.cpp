#include "../profile/hud/delusiontrigger.h"
#include "../profile/configsystem.h"
#include "../text.h"

namespace Impacto {
namespace DelusionTrigger {

DelusionTriggerBase::DelusionTriggerBase(int& delusionState, UiState showState)
    : State(showState), DelusionState(delusionState) {};

void Init() { Profile::DelusionTrigger::Configure(); }

void Show() {
  if (Implementation) {
    if (Profile::ConfigSystem::TriggerStopSkip)
      MesSkipMode &= SkipModeFlags::Auto;

    Implementation->Show();
  }
}

bool Show(int bgMtrgSelBufferId, int bgMtrgNegaPosiBufferId, int param3) {
  if (Implementation) {
    if (Profile::ConfigSystem::TriggerStopSkip)
      MesSkipMode &= SkipModeFlags::Auto;

    return Implementation->Show(bgMtrgSelBufferId, bgMtrgNegaPosiBufferId,
                                param3);
  }
  return true;
}

bool CheckTransitionAnimationComplete() {
  if (Implementation) return Implementation->CheckTransitionAnimationComplete();
  return true;
}

bool CheckStartTransitionComplete() {
  if (Implementation) return Implementation->CheckStartTransitionComplete();
  return true;
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