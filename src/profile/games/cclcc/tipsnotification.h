#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TipsNotification {

void Configure();

extern Sprite NotificationBackground;

extern glm::vec2 BackgroundPosition;
extern glm::vec2 InitialNotificationPosition;
extern RectF NotificationRenderingBounds;
extern float TimerDuration;
extern float MoveAnimationDuration;

extern int TipNameColorIndex;
extern float FontSize;

}  // namespace TipsNotification
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto