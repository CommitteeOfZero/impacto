#include "../profile/hud/delusiontrigger.h"

namespace Impacto {
namespace DelusionTrigger {

DelusionTriggerBase::DelusionTriggerBase(int& delusionState, UiState showState)
    : DelusionState(delusionState), State(showState){};

DelusionTriggerBase* Implementation = 0;

void Init() { Profile::DelusionTrigger::Configure(); }

void Show() {
  if (Implementation) Implementation->Show();
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