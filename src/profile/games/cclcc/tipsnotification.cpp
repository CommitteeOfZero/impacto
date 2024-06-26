#include "tipsnotification.h"
#include "../../../log.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TipsNotification {

void Configure() {
  NotificationBackground = EnsureGetMemberSprite("NotificationBackground");

  BackgroundPosition = EnsureGetMemberVec2("BackgroundPosition");
  InitialNotificationPosition =
      EnsureGetMemberVec2("InitialNotificationPosition");
  NotificationRenderingBounds =
      EnsureGetMemberRectF("NotificationRenderingBounds");
  TimerDuration = EnsureGetMemberFloat("TimerDuration");

  TipNameColorIndex = EnsureGetMemberInt("TipNameColorIndex");
  FontSize = EnsureGetMemberFloat("FontSize");
}

}  // namespace TipsNotification
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto