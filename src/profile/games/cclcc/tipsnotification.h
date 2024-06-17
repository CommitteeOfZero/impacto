#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TipsNotification {

void Configure();

inline Sprite NotificationBackground;

inline glm::vec2 BackgroundPosition;
inline glm::vec2 InitialNotificationPosition;
inline RectF NotificationRenderingBounds;
inline float TimerDuration;
inline float MoveAnimationDuration;

inline int TipNameColorIndex;
inline float FontSize;

}  // namespace TipsNotification
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto