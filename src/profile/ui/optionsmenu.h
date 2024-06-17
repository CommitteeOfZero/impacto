#pragma once

#include "../../ui/ui.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace OptionsMenu {

inline Impacto::UI::OptionsMenuType Type = Impacto::UI::OptionsMenuType::None;

inline Sprite BackgroundSprite;
inline Sprite SliderTrackSprite;
inline Sprite SliderFillSprite;
inline Sprite SliderThumbSprite;

inline float FadeInDuration;
inline float FadeOutDuration;

void Configure();

}  // namespace OptionsMenu
}  // namespace Profile
}  // namespace Impacto