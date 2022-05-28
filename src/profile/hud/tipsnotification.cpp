#include "tipsnotification.h"
#include "../profile_internal.h"
#include "../games/mo6tw/tipsnotification.h"
#include "../games/cclcc/tipsnotification.h"
#include "../../games/mo6tw/tipsnotification.h"
#include "../../games/cclcc/tipsnotification.h"

namespace Impacto {
namespace Profile {
namespace TipsNotification {

using namespace Impacto::TipsNotification;

TipsNotificationType Type = TipsNotificationType::None;

int TextTableId;
int NotificationAlertMessageId;
int NotificationTextPart1MessageId;
int NotificationTextPart2MessageId;

float FadeInDuration;
float FadeOutDuration;

void Configure() {
  EnsurePushMemberOfType("TipsNotification", kObjectType);

  Type = TipsNotificationType::_from_integral_unchecked(
      EnsureGetMemberInt("Type"));

  switch (Type) {
    case TipsNotificationType::MO6TW:
      MO6TW::TipsNotification::Configure();
      break;
    case TipsNotificationType::CCLCC:
      CCLCC::TipsNotification::Configure();
      break;
    default:
      Pop();
      return;
  }

  TextTableId = EnsureGetMemberInt("TextTableId");
  NotificationAlertMessageId = EnsureGetMemberInt("NotificationAlertMessageId");
  NotificationTextPart1MessageId =
      EnsureGetMemberInt("NotificationTextPart1MessageId");
  NotificationTextPart2MessageId =
      EnsureGetMemberInt("NotificationTextPart2MessageId");

  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

  Pop();
}

void CreateInstance() {
  if (!Impacto::TipsNotification::Implementation) {
    switch (Type) {
      case TipsNotificationType::MO6TW:
        Impacto::TipsNotification::Implementation =
            new Impacto::MO6TW::TipsNotification;
        break;
      // case TipsNotificationType::CCLCC:
      //   Impacto::TipsNotification::Implementation =
      //       new Impacto::CCLCC::TipsNotification;
      //   break;
      default:
        return;
    }
  }
}

}  // namespace TipsNotification
}  // namespace Profile
}  // namespace Impacto