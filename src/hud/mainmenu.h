#pragma once

#include <enum.h>
#include "../animation.h"

namespace Impacto {
namespace MainMenu {

BETTER_ENUM(MainMenuType, int, None, RNE)

extern Animation* BackgroundAnimation;

enum MainMenuState { Hidden, Hiding, Showing, Shown };

extern MainMenuState State;

void Init();
void Show();
void Hide();
void Update(float dt);
void Render();

}  // namespace MainMenu
}  // namespace Impacto