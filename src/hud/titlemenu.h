#pragma once

#include <enum.h>
#include "../animation.h"

namespace Impacto {
namespace TitleMenu {

BETTER_ENUM(TitleMenuType, int, None, RNE, Dash, CHLCC)

enum TitleMenuState { Hidden, Hiding, Showing, Shown };

extern TitleMenuState State;

class TitleMenuBase {
 public:
  virtual void Show() = 0;
  virtual void Hide() = 0;
  virtual void Update(float dt) = 0;
  virtual void Render() = 0;

  Animation PressToStartAnimation;

 protected:
  TitleMenuState State;
};

extern TitleMenuBase* Implementation;

void Init();
void Show();
void Hide();
void Update(float dt);
void Render();

}  // namespace TitleMenu
}  // namespace Impacto