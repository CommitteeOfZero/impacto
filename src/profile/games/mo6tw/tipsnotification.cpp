#include "tipsnotification.h"
#include "../../../log.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TipsNotification {

void Configure() {
  AlertPosition = EnsureGetMember<glm::vec2>("AlertPosition");
  NotificationAlertMessageId =
      EnsureGetMember<int>("NotificationAlertMessageId");
  FinalNotificationPosition =
      EnsureGetMember<glm::vec2>("FinalNotificationPosition");
  InitialNotificationPosition =
      EnsureGetMember<glm::vec2>("InitialNotificationPosition");
  NotificationRenderingBounds =
      EnsureGetMember<RectF>("NotificationRenderingBounds");
  TimerDuration = EnsureGetMember<float>("TimerDuration");
  MoveAnimationDuration = EnsureGetMember<float>("MoveAnimationDuration");

  AlertTextColorIndex = EnsureGetMember<int>("AlertTextColorIndex");
  TipNameColorIndex = EnsureGetMember<int>("TipNameColorIndex");
  FontSize = EnsureGetMember<float>("FontSize");
}

}  // namespace TipsNotification
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto