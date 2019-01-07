#pragma once

#include <enum.h>
#include "../animation.h"

namespace Impacto {
namespace MainMenu {

BETTER_ENUM(MainMenuType, int, None, RNE)

enum MainMenuState { Hidden, Hiding, Showing, Shown };

extern MainMenuState State;

class MainMenuBase {
 public:
  virtual void Show() = 0;
  virtual void Hide() = 0;
  virtual void Update(float dt) = 0;
  virtual void Render() = 0;

 protected:
  MainMenuState State;
};

extern MainMenuBase* Implementation;

void Init();
void Show();
void Hide();
void Update(float dt);
void Render();

}  // namespace MainMenu
}  // namespace Impacto