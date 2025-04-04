#include "datedisplay.h"

#include "../profile/configsystem.h"
#include "../profile/hud/tipsnotification.h"

namespace Impacto {
namespace TipsNotification {

void Init() { Profile::TipsNotification::Configure(); }

void Update(float dt) {
  if (Implementation) Implementation->Update(dt);
}
void Render() {
  if (Implementation) Implementation->Render();
}

void AddTip(int tipId) {
  if (Implementation && Profile::ConfigSystem::ShowTipsNotification)
    Implementation->AddTip(tipId);
}

}  // namespace TipsNotification
}  // namespace Impacto