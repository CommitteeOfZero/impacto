#pragma once

#include "../../hud/mainmenu.h"

namespace Impacto {
namespace MO6TW {

using namespace Impacto::MainMenu;

class MainMenu : public MainMenuBase {
 public:
  void Show();
  void Hide();
  void Update(float dt);
  void Render();
};

}  // namespace MO6TW
}  // namespace Impacto