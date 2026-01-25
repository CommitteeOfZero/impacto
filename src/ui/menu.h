#pragma once

#include <vector>
#include "../impacto.h"
#include "widget.h"
#include "../game.h"

namespace Impacto {
namespace UI {

enum MenuState : uint8_t { Hidden, Hiding, Showing, Shown };
enum class InputRate : uint8_t { SingleTap, RepeatSlow, RepeatFast, Hold };

class Menu {
 public:
  virtual void Init() {};
  virtual void Show();
  virtual void Hide();
  virtual void Update(float dt) = 0;
  virtual void Render() = 0;

  virtual void UpdateInput(float dt);

  Menu* LastFocusedMenu = 0;
  Widget* FocusStart[4] = {0, 0, 0, 0};
  Widget* CurrentlyFocusedElement = 0;
  InputRate InputConfig = InputRate::RepeatSlow;
  MenuState State = Hidden;

  bool IsFocused = false;
  bool ChoiceMade = false;
  bool AllowsScriptInput = true;
  uint8_t DrawType = Game::DrawComponentType::Main;

 protected:
  void AdvanceFocus(FocusDirection dir);
};

}  // namespace UI
}  // namespace Impacto