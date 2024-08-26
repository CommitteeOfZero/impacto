#pragma once

#include "../../../text.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TipsNotification {

void Configure();

inline Sprite NotificationBackground;

inline float BackgroundPositionX;
inline float BackgroundPositionYOffset;
inline float NotificationPositionX;
inline float NotificationPositionYOffset;
inline float TimerDuration;
inline float MoveAnimationDuration;

inline DialogueColorPair TipNameColor;
inline int NotificationTextTableColorIndex;
inline float FontSize;

}  // namespace TipsNotification
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto