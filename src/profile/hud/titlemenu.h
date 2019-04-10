#pragma once

#include "../../hud/titlemenu.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace TitleMenu {

extern Impacto::TitleMenu::TitleMenuType Type;

extern Sprite PressToStartSprite;

extern float PressToStartAnimDurationIn;
extern float PressToStartAnimDurationOut;

extern float PressToStartX;
extern float PressToStartY;

void Configure();

}  // namespace TitleMenu
}  // namespace Profile
}  // namespace Impacto