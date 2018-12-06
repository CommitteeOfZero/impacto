#include "mainmenu.h"

#include "../profile/hud/mainmenu.h"

namespace Impacto {
namespace MainMenu {

Animation* BackgroundAnimation = 0;

void Init() { Profile::MainMenu::Configure(); }

void Show() {
  if (BackgroundAnimation) BackgroundAnimation->StartIn();
}
void Hide() {
  if (BackgroundAnimation) BackgroundAnimation->StartOut();
}
void Update(float dt) {
  if (BackgroundAnimation) BackgroundAnimation->Update(dt);
}
void Render() {
  if (BackgroundAnimation) BackgroundAnimation->Render();
}

}  // namespace MainMenu
}  // namespace Impacto