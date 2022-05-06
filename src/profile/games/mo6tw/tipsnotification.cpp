#include "tipsnotification.h"
#include "../../../log.h"
#include "../../../window.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TipsNotification {

glm::vec2 AlertPosition;
glm::vec2 FinalNotificationPosition;
glm::vec2 InitialNotificationPosition;
RectF NotificationRenderingBounds;
float TimerDuration;
float MoveAnimationDuration;

int AlertTextColorIndex;
int TipNameColorIndex;
int FontSize;

void Configure() {
  AlertPosition = EnsureGetMemberVec2("AlertPosition");
  FinalNotificationPosition = EnsureGetMemberVec2("FinalNotificationPosition");
  InitialNotificationPosition =
      EnsureGetMemberVec2("InitialNotificationPosition");
  NotificationRenderingBounds =
      EnsureGetMemberRectF("NotificationRenderingBounds");
  TimerDuration = EnsureGetMemberFloat("TimerDuration");
  MoveAnimationDuration = EnsureGetMemberFloat("MoveAnimationDuration");

  AlertTextColorIndex = EnsureGetMemberInt("AlertTextColorIndex");
  TipNameColorIndex = EnsureGetMemberInt("TipNameColorIndex");
  FontSize = EnsureGetMemberInt("FontSize");
}

}  // namespace TipsNotification
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto