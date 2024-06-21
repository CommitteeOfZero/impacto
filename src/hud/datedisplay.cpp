#include "datedisplay.h"

#include "../profile/hud/datedisplay.h"

namespace Impacto {
namespace DateDisplay {

void Init() {
  Profile::DateDisplay::Configure();
  if (Implementation) {
    Implementation->FadeAnimation.DurationIn =
        Profile::DateDisplay::FadeInDuration;
    Implementation->FadeAnimation.DurationOut =
        Profile::DateDisplay::FadeOutDuration;
  }
}

void Update(float dt) {
  if (Implementation) Implementation->Update(dt);
}
void Render() {
  if (Implementation) Implementation->Render();
}

}  // namespace DateDisplay
}  // namespace Impacto