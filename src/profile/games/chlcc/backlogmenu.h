#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace BacklogMenu {

void Configure();

inline float TransitionDuration;
inline uint32_t BackgroundColor;
inline Sprite CircleSprite;
inline glm::vec2 CircleStartPosition;
inline float CircleOffset;
inline Sprite ErinSprite;
inline glm::vec2 ErinPosition;
inline Sprite BackgroundFilter;
inline glm::vec2 InitialRedBarPosition;
inline Sprite InitialRedBarSprite;
inline glm::vec2 RightRedBarPosition;
inline glm::vec2 RedBarPosition;
inline float RedBarDivision;
inline float RedBarBaseX;
inline Sprite RedBarSprite;
inline glm::vec2 RedBarLabelPosition;
inline Sprite RedBarLabel;
inline glm::vec2 MenuTitleTextRightPosition;
inline glm::vec2 MenuTitleTextLeftPosition;
inline float MenuTitleTextAngle;
inline float TitleFadeInDuration;
inline float TitleFadeOutDuration;
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