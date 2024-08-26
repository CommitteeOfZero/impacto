#include "tipsnotification.h"
#include "../../../log.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TipsNotification {

void Configure() {
  NotificationBackground = EnsureGetMemberSprite("NotificationBackground");

  BackgroundPositionX = EnsureGetMemberFloat("BackgroundPositionX");
  BackgroundPositionYOffset = EnsureGetMemberFloat("BackgroundPositionYOffset");
  NotificationPositionX = EnsureGetMemberFloat("NotificationPositionX");
  NotificationPositionYOffset =
      EnsureGetMemberFloat("NotificationPositionYOffset");
  TimerDuration = EnsureGetMemberFloat("TimerDuration");

  FontSize = EnsureGetMemberFloat("FontSize");
  TipNameColor = DialogueColorPair{EnsureGetMemberUint("TipNameTextColor"),
                                   EnsureGetMemberUint("TipNameOutlineColor")};
  NotificationTextTableColorIndex =
      EnsureGetMemberInt("NotificationTextTableColorIndex");
}

}  // namespace TipsNotification
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto