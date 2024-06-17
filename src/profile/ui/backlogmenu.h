#pragma once

#include "../../ui/ui.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace BacklogMenu {

inline Sprite BacklogBackground;
inline Sprite EntryHighlight;
inline Sprite VoiceIcon;
inline Sprite ScrollbarTrack;
inline Sprite ScrollbarThumb;

inline float EntryYPadding;
inline glm::vec2 EntriesStart;
inline glm::vec2 ScrollbarPosition;
inline RectF RenderingBounds;

inline float FadeInDuration;
inline float FadeOutDuration;

void Configure();

}  // namespace BacklogMenu
}  // namespace Profile
}  // namespace Impacto