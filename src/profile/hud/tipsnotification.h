#pragma once

#include "../sprites.h"
#include "../../hud/tipsnotification.h"

namespace Impacto {
namespace Profile {
namespace TipsNotification {

extern int TextTableId;
extern int NotificationAlertMessageId;
extern int NotificationTextPart1MessageId;
extern int NotificationTextPart2MessageId;

extern float FadeInDuration;
extern float FadeOutDuration;

void Configure();

void CreateInstance();

}  // namespace TipsNotification
}  // namespace Profile
}  // namespace Impacto