#pragma once

#include "../../ui/ui.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace TipsMenu {

inline Impacto::UI::TipsMenuType Type = Impacto::UI::TipsMenuType::None;

inline Sprite BackgroundSprite;

inline float FadeInDuration;
inline float FadeOutDuration;

void Configure();

}  // namespace TipsMenu
}  // namespace Profile
}  // namespace Impacto