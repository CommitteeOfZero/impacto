#pragma once

#include "../../ui/ui.h"
#include "../../spritesheet.h"
#include <enum.h>

namespace Impacto {
namespace Profile {
namespace BacklogMenu {

BETTER_ENUM(BacklogMenuType, int, None, MO6TW, CHLCC, CC)
BETTER_ENUM(EntryHighlightLocationType, int, None, BottomLeftOfEntry,
            TopLineLeftOfScreen, AllLinesLeftOfScreen)

inline BacklogMenuType Type = BacklogMenuType::None;
inline EntryHighlightLocationType EntryHighlightLocation =
    EntryHighlightLocationType::None;

inline Sprite BacklogBackground;
inline Sprite EntryHighlight;
inline Sprite VoiceIcon;
inline Sprite ScrollbarTrack;
inline Sprite ScrollbarThumb;

inline float EntryYPadding;
inline glm::vec2 EntriesStart;
inline glm::vec2 EntryHighlightOffset;
inline float EntryHighlightPadding;
inline glm::vec2 VoiceIconOffset;
inline glm::vec2 ScrollbarPosition;
inline float ScrollbarThumbLength;
inline RectF RenderingBounds;
inline RectF HoverBounds;

inline float FadeInDuration;
inline float FadeOutDuration;

inline float ScrollingSpeed;
inline float MinHoldTime;
inline float AdvanceFocusTimeInterval;
inline float PageUpDownHeight;

void Configure();

}  // namespace BacklogMenu
}  // namespace Profile
}  // namespace Impacto