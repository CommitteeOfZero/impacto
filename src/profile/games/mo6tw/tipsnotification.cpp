#include "tipsnotification.h"
#include "../../../log.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace TipsNotification {

void Configure() {
  AlertPosition = EnsureGetMemberVec2("AlertPosition");
  NotificationAlertMessageId = EnsureGetMemberInt("NotificationAlertMessageId");
  FinalNotificationPosition = EnsureGetMemberVec2("FinalNotificationPosition");
  InitialNotificationPosition =
      EnsureGetMemberVec2("InitialNotificationPosition");
  NotificationRenderingBounds =
      EnsureGetMemberRectF("NotificationRenderingBounds");
  TimerDuration = EnsureGetMemberFloat("TimerDuration");
  MoveAnimationDuration = EnsureGetMemberFloat("MoveAnimationDuration");

  AlertTextColorIndex = EnsureGetMemberInt("AlertTextColorIndex");
  TipNameColorIndex = EnsureGetMemberInt("TipNameColorIndex");
  FontSize = EnsureGetMemberFloat("FontSize");
}

}  // namespace TipsNotification
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto