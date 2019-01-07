#pragma once

#include "../../hud/mainmenu.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MainMenu {

extern Impacto::MainMenu::MainMenuType Type;

static int const MenuEntriesNum = 8;

extern Sprite ButtonBackgroundSprite;
extern Sprite ButtonPromptsSprite;
extern Sprite MenuEntriesSprites[8];
extern Sprite MenuEntriesHSprites[8];
extern float ButtonBackgroundStartX;
extern float ButtonBackgroundX;
extern float ButtonBackgroundY;
extern float ButtonBackgroundTargetWidth;
extern float ButtonBackgroundSprStartX;
extern float MenuEntriesX;
extern float MenuEntriesXOffset;
extern float MenuEntriesFirstY;
extern float MenuEntriesYPadding;

void Configure();

}  // namespace MainMenu
}  // namespace Profile
}  // namespace Impacto