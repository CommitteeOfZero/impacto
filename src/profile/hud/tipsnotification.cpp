#include "tipsnotification.h"
#include "../profile_internal.h"
#include "../games/mo6tw/tipsnotification.h"
#include "../games/cclcc/tipsnotification.h"
#include "../games/chlcc/tipsnotification.h"
#include "../../hud/mo6tw/tipsnotification.h"
#include "../../hud/cclcc/tipsnotification.h"
#include "../../hud/chlcc/tipsnotification.h"

namespace Impacto {
namespace Profile {
namespace TipsNotification {

using namespace Impacto::TipsNotification;

TipsNotificationType Type = TipsNotificationType::None;

void Configure() {
  EnsurePushMemberOfType("TipsNotification", LUA_TTABLE);

  Type = TipsNotificationType::_from_integral_unchecked(
      EnsureGetMember<int>("Type"));

  switch (Type) {
    case TipsNotificationType::MO6TW:
      MO6TW::TipsNotification::Configure();
      break;
    case TipsNotificationType::CCLCC:
      CCLCC::TipsNotification::Configure();
      break;
    case TipsNotificationType::CHLCC:
      CHLCC::TipsNotification::Configure();
      break;
    default:
      Pop();
      return;
  }

  TextTableId = EnsureGetMember<int>("TextTableId");
  NotificationTextPart1MessageId =
      EnsureGetMember<int>("NotificationTextPart1MessageId");
  NotificationTextPart2MessageId =
      EnsureGetMember<int>("NotificationTextPart2MessageId");

  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

  Pop();
}

void CreateInstance() {
  switch (Type) {
    case TipsNotificationType::MO6TW:
      Impacto::TipsNotification::Implementation =
          std::make_unique<Impacto::MO6TW::TipsNotification>();
      break;
    case TipsNotificationType::CCLCC:
      Impacto::TipsNotification::Implementation =
          std::make_unique<Impacto::CCLCC::TipsNotification>();
      break;
    case TipsNotificationType::CHLCC:
      Impacto::TipsNotification::Implementation =
          std::make_unique<Impacto::CHLCC::TipsNotification>();
      break;
    default:
      return;
  }
}

}  // namespace TipsNotification
}  // namespace Profile
}  // namespace Impacto