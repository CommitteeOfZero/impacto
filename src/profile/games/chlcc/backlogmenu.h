#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace BacklogMenu {

void Configure();

inline uint32_t BackgroundColor;
inline Sprite CircleSprite;
inline glm::vec2 MenuTitleTextRightPosition;
inline glm::vec2 MenuTitleTextLeftPosition;
inline float MenuTitleTextAngle;
inline Sprite MenuTitleText;

inline glm::vec2 ButtonPromptPosition;
inline Sprite ButtonPromptSprite;
inline Sprite BacklogBackgroundSprite;
inline Sprite EntryHighlightSprite;
inline Sprite VoiceIconSprite;
inline Sprite ScrollbarThumbSprite;
inline glm::vec2 ScrollbarPosition;
inline glm::vec2 EntriesStart;
inline RectF RenderingBounds;
inline float EntryYPadding;

}  // namespace BacklogMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto