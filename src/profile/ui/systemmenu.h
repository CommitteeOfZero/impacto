#pragma once

#include "../../ui/ui.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace SystemMenu {

inline Impacto::UI::SystemMenuType Type = Impacto::UI::SystemMenuType::None;

int constexpr MenuEntriesNumMax = 16;

inline Sprite MenuEntriesSprites[MenuEntriesNumMax];
inline Sprite MenuEntriesHSprites[MenuEntriesNumMax];
inline int MenuEntriesNum;
inline int MenuEntriesHNum;
inline float MenuEntriesX;
inline float MenuEntriesXOffset;
inline float MenuEntriesFirstY;
inline float MenuEntriesYPadding;
inline float FadeInDuration = 0.5f;
inline float FadeOutDuration = 0.5f;

void Configure();

}  // namespace SystemMenu
}  // namespace Profile
}  // namespace Impacto