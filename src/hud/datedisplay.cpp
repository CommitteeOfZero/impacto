#include "datedisplay.h"

#include "../profile/hud/datedisplay.h"

namespace Impacto {
namespace DateDisplay {

DateDisplayBase* Implementation = 0;

void Init() {
  Profile::DateDisplay::Configure();
  if (Implementation) {
    Implementation->FadeAnimation.DurationIn =
        Profile::DateDisplay::FadeInDuration;
    Implementation->FadeAnimation.DurationOut =
        Profile::DateDisplay::FadeOutDuration;
  }
}

void Update(float dt) { Implementation->Update(dt); }
void Render() { Implementation->Render(); }

}  // namespace DateDisplay
}  // namespace Impacto