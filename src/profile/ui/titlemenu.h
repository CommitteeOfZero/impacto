#pragma once

#include "../../ui/ui.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace TitleMenu {

constexpr int MenuEntriesNumMax = 32;

extern Impacto::UI::TitleMenuType Type;

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