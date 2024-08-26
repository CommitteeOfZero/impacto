#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TipsNotification {

void Configure();

inline glm::vec2 AlertPosition;
inline int NotificationAlertMessageId;
inline glm::vec2 FinalNotificationPosition;
inline glm::vec2 InitialNotificationPosition;
inline RectF NotificationRenderingBounds;
inline float TimerDuration;
inline float MoveAnimationDuration;

inline int AlertTextColorIndex;
inline int TipNameColorIndex;
inline float FontSize;

}  // namespace TipsNotification
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto