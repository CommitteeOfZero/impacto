#pragma once

#include "../../ui/ui.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace SaveMenu {

inline Impacto::UI::SaveMenuType Type = Impacto::UI::SaveMenuType::None;

inline Sprite EmptyThumbnailSprite;

inline float FadeInDuration;
inline float FadeOutDuration;

void Configure();

}  // namespace SaveMenu
}  // namespace Profile
}  // namespace Impacto