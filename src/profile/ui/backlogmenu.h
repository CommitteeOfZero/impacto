#pragma once

#include "../../ui/ui.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace BacklogMenu {

extern Sprite BacklogBackground;
extern Sprite EntryHighlight;
extern Sprite VoiceIcon;
extern Sprite ScrollbarTrack;
extern Sprite ScrollbarThumb;

extern float EntryYPadding;
extern glm::vec2 EntriesStart;
extern glm::vec2 ScrollbarPosition;
extern RectF RenderingBounds;

extern float FadeInDuration;
extern float FadeOutDuration;

void Configure();

}  // namespace BacklogMenu
}  // namespace Profile
}  // namespace Impacto