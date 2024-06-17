#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO8 {
namespace SystemMenu {

void Configure();

int constexpr MenuEntriesNumMax = 16;

inline Sprite SystemMenuBackgroundSprite;
inline Sprite MenuEntriesLSprites[MenuEntriesNumMax];
inline int MenuEntriesLNum;

inline int ExitMenuButtonId;

inline float MenuEntriesTargetWidth;
inline float SystemMenuX;
inline float SystemMenuY;

}  // namespace SystemMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto