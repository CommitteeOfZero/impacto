#pragma once

#include "../../hud/mainmenu.h"

namespace Impacto {
namespace RNE {

using namespace Impacto::MainMenu;

class MainMenu : public MainMenuBase {
 public:
  void Show();
  void Hide();
  void Update(float dt);
  void Render();

  Animation* BackgroundAnimation = 0;
  Animation SkyMoveAnimation;
  Animation EntriesMoveAnimation;
  Animation HighlightAnimation;
};

}  // namespace RNE
}  // namespace Impacto