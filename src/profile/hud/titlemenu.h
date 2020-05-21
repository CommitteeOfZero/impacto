#pragma once

#include "../../hud/titlemenu.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace TitleMenu {

static int const MenuEntriesNumMax = 16;

extern Impacto::TitleMenu::TitleMenuType Type;

extern Sprite PressToStartSprite;
extern Sprite MenuEntriesSprites[MenuEntriesNumMax];
extern Sprite MenuEntriesHSprites[MenuEntriesNumMax];

extern int MenuEntriesNum;

extern float PressToStartAnimDurationIn;
extern float PressToStartAnimDurationOut;

extern float PressToStartX;
extern float PressToStartY;

void Configure();

}  // namespace TitleMenu
}  // namespace Profile
}  // namespace Impacto