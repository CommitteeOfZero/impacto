#pragma once

#include <enum.h>
#include "../animation.h"

namespace Impacto {
namespace TitleMenu {

BETTER_ENUM(TitleMenuType, int, None, RNE)

extern Animation* BackgroundAnimation;

enum TitleMenuState { Hidden, Hiding, Showing, Shown };

extern TitleMenuState State;

void Init();
void Show();
void Hide();
void Update(float dt);
void Render();

}  // namespace TitleMenu
}  // namespace Impacto