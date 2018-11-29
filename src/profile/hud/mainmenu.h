#pragma once

#include "../../hud/mainmenu.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MainMenu {

extern Impacto::MainMenu::MainMenuType Type;

extern float BackgroundInDuration;
extern float BackgroundOutDuration;
extern Sprite BackgroundSprite;

void Configure();

}  // namespace MainMenu
}  // namespace Profile
}  // namespace Impacto