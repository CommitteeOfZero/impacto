#pragma once

#include "../../ui/ui.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace OptionsMenu {

extern Impacto::UI::OptionsMenuType Type;

extern Sprite BackgroundSprite;
extern Sprite SliderTrackSprite;
extern Sprite SliderFillSprite;
extern Sprite SliderThumbSprite;

extern float FadeInDuration;
extern float FadeOutDuration;

void Configure();

}  // namespace OptionsMenu
}  // namespace Profile
}  // namespace Impacto