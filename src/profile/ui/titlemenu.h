#pragma once

#include "../../ui/ui.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace TitleMenu {

int constexpr MenuEntriesNumMax = 32;

inline Impacto::UI::TitleMenuType Type = Impacto::UI::TitleMenuType::None;

inline Sprite PressToStartSprite;
inline Sprite MenuEntriesSprites[MenuEntriesNumMax];
inline Sprite MenuEntriesHSprites[MenuEntriesNumMax];

inline int MenuEntriesNum;

inline float PressToStartAnimDurationIn;
inline float PressToStartAnimDurationOut;

inline glm::vec2 PressToStartPos;

void Configure();

}  // namespace TitleMenu
}  // namespace Profile
}  // namespace Impacto