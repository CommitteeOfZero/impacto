#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO8 {
namespace SystemMenu {

void Configure();

constexpr int MenuEntriesNumMax = 16;

extern Sprite SystemMenuBackgroundSprite;
extern Sprite MenuEntriesLSprites[MenuEntriesNumMax];
extern int MenuEntriesLNum;

extern int ExitMenuButtonId;

extern float MenuEntriesTargetWidth;
extern float SystemMenuX;
extern float SystemMenuY;

}  // namespace SystemMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto