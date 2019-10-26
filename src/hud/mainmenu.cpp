#include "mainmenu.h"

#include "../profile/hud/mainmenu.h"
#include "../renderer2d.h"
#include "../games/rne/tilebackground.h"
#include "../mem.h"

namespace Impacto {
namespace MainMenu {

using namespace Impacto::Profile::MainMenu;

MainMenuBase* Implementation = 0;

void Init() { Configure(); }

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

}  // namespace MainMenu
}  // namespace Impacto