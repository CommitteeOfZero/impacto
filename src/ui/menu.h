#pragma once

#include <vector>
#include "../impacto.h"
#include "widget.h"
#include "../game.h"

namespace Impacto {
namespace UI {

enum MenuState { Hidden, Hiding, Showing, Shown };

class Menu {
 public:
  virtual void Show();
  virtual void Hide();
  virtual void Update(float dt) = 0;
  virtual void Render() = 0;

  void UpdateInput();

  MenuState State = Hidden;

  bool IsFocused = false;
  bool ChoiceMade = false;

  Menu* LastFocusedMenu = 0;
  Widget* FocusStart[4] = {0, 0, 0, 0};
  Widget* CurrentlyFocusedElement = 0;

  uint8_t DrawType = Game::DrawComponentType::Main;

 private:
  void AdvanceFocus(FocusDirection dir);
};

}  // namespace UI
}  // namespace Impacto