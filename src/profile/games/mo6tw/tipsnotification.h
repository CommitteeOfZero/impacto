#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TipsNotification {

void Configure();

extern glm::vec2 AlertPosition;
extern glm::vec2 FinalNotificationPosition;
extern glm::vec2 InitialNotificationPosition;
extern RectF NotificationRenderingBounds;
extern float TimerDuration;
extern float MoveAnimationDuration;

extern int AlertTextColorIndex;
extern int TipNameColorIndex;
extern float FontSize;

}  // namespace TipsNotification
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto