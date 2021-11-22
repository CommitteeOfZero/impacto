#pragma once

#include "../../ui/ui.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace TipsMenu {

extern Impacto::UI::TipsMenuType Type;

extern Sprite BackgroundSprite;

extern float FadeInDuration;
extern float FadeOutDuration;

void Configure();

}  // namespace TipsMenu
}  // namespace Profile
}  // namespace Impacto