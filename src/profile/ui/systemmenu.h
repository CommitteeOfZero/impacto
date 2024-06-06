#pragma once

#include "../../ui/ui.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace SystemMenu {

extern Impacto::UI::SystemMenuType Type;

constexpr int MenuEntriesNumMax = 16;

extern Sprite MenuEntriesSprites[MenuEntriesNumMax];
extern Sprite MenuEntriesHSprites[MenuEntriesNumMax];
extern int MenuEntriesNum;
extern int MenuEntriesHNum;
extern float MenuEntriesX;
extern float MenuEntriesXOffset;
extern float MenuEntriesFirstY;
extern float MenuEntriesYPadding;
extern float FadeInDuration;
extern float FadeOutDuration;

void Configure();

}  // namespace SystemMenu
}  // namespace Profile
}  // namespace Impacto