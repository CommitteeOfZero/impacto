#pragma once

#include "../../ui/ui.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace SaveMenu {

extern Impacto::UI::SaveMenuType Type;

extern Sprite SaveMenuBackgroundSprite;
extern Sprite EmptyThumbnailSprite;

extern float EntryStartX;
extern float EntryXPadding;
extern float EntryStartY;
extern float EntryYPadding;

extern float FadeInDuration;
extern float FadeOutDuration;

void Configure();

}  // namespace SaveMenu
}  // namespace Profile
}  // namespace Impacto