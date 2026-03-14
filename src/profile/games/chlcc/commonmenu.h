#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace CommonMenu {
inline glm::vec2 CircleStartPosition;
inline float CircleOffset;

inline glm::vec2 ErinPosition;
inline Sprite ErinSprite;

inline Sprite BackgroundFilter;

inline float TitleFadeInDuration;
inline float TitleFadeOutDuration;

inline Sprite InitialRedBarSprite;
inline Sprite RedBarSprite;

inline glm::vec2 InitialRedBarPosition;
inline glm::vec2 RedBarPosition;

inline glm::vec2 RightRedBarPosition;
inline float RedBarDivision;
inline float RedBarBaseX;
inline glm::vec2 RedBarLabelPosition;
inline Sprite RedBarLabel;
inline glm::vec2 DiagonalTitlesOffsetStart;
inline glm::vec2 DiagonalTitlesOffsetEnd;

inline float MenuSelectPromptDuration;
inline float MenuSelectPromptInterval;
inline float MenuTransitionDuration;
inline float ShowPageAnimationStartTime;
inline float ShowPageAnimationDuration;
inline float ButtonPromptAnimationStartTime;
inline float ButtonPromptAnimationDuration;
inline glm::vec2 ButtonPromptStartPosition;

void Configure();

}  // namespace CommonMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto