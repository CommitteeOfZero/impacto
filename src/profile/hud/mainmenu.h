#pragma once

#include "../../hud/mainmenu.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MainMenu {

extern Impacto::MainMenu::MainMenuType Type;

static int const MenuEntriesNum = 8;

extern Sprite ButtonBackgroundSprite;
extern Sprite SkyBackgroundSprite;
extern Sprite SkyArrowSprite;
extern Sprite SkyTextSprite;
extern Sprite ButtonPromptsSprite;
extern Sprite MenuEntriesSprites[8];
extern Sprite MenuEntriesHSprites[8];
extern float SkyBackgroundBeginX;
extern float SkyBackgroundY;
extern float SkyTextBeginX;
extern float SkyTextY;
extern float ButtonBackgroundStartX;
extern float ButtonBackgroundX;
extern float ButtonBackgroundY;
extern float ButtonBackgroundTargetWidth;
extern float ButtonBackgroundSprStartX;
extern float MenuEntriesX;
extern float MenuEntriesXSkew;
extern float MenuEntriesXOffset;
extern float MenuEntriesFirstY;
extern float MenuEntriesYPadding;
extern float MenuEntriesTargetWidth;
extern float SkyInStartProgress;
extern float SkyOutStartProgress;
extern float SkyMoveDurationIn;
extern float SkyMoveDurationOut;
extern float EntriesMoveDurationIn;
extern float EntriesMoveDurationOut;
extern float HighlightDurationIn;
extern float HighlightDurationOut;

void Configure();

}  // namespace MainMenu
}  // namespace Profile
}  // namespace Impacto