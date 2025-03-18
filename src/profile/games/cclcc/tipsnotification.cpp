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
  TipNameColor =
      DialogueColorPair{EnsureGetMember<uint32_t>("TipNameTextColor"),
                        EnsureGetMember<uint32_t>("TipNameOutlineColor")};
  NotificationTextTableColorIndex =
      EnsureGetMember<int>("NotificationTextTableColorIndex");
}

}  // namespace TipsNotification
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto