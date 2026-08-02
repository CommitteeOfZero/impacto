#include "tipsnotification.h"
#include "../../../log.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TipsNotification {

void Configure() {
  NotificationBackground = EnsureGetMember<Sprite>("NotificationBackground");

  BackgroundPositionX = EnsureGetMember<float>("BackgroundPositionX");
  BackgroundPositionYOffset =
      EnsureGetMember<float>("BackgroundPositionYOffset");
  NotificationPositionX = EnsureGetMember<float>("NotificationPositionX");
  NotificationPositionYOffset =
      EnsureGetMember<float>("NotificationPositionYOffset");
  TimerDuration = EnsureGetMember<float>("TimerDuration");

  FontSize = EnsureGetMember<float>("FontSize");
  TipNameColor = EnsureGetMember<DialogueColorPair>("TipNameColor");
  NotificationTextColor =
      EnsureGetMember<DialogueColorPair>("NotificationTextColor");
}

}  // namespace TipsNotification
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto