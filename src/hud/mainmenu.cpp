#include "mainmenu.h"

#include "../profile/hud/mainmenu.h"
#include "../animation.h"
#include "../games/rne/tilebackground.h"

namespace Impacto {
namespace MainMenu {

static Animation* BackgroundAnimation = 0;

void Init() {
  Profile::MainMenu::Configure();

  switch (Profile::MainMenu::Type) {
    case MainMenuType::RNE: {
      BackgroundAnimation = new RNE::TileBackground();
      break;
    }
  }
}

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