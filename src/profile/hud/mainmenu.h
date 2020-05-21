#pragma once

#include "../../hud/mainmenu.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MainMenu {

extern Impacto::MainMenu::MainMenuType Type;

static int const MenuEntriesNumMax = 16;

extern Sprite MenuEntriesSprites[MenuEntriesNumMax];
extern Sprite MenuEntriesHSprites[MenuEntriesNumMax];
extern int MenuEntriesNum;
extern int MenuEntriesHNum;
extern float MenuEntriesX;
extern float MenuEntriesXOffset;
extern float MenuEntriesFirstY;
extern float MenuEntriesYPadding;

void Configure();

}  // namespace MainMenu
}  // namespace Profile
}  // namespace Impacto