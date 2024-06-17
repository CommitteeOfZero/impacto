#pragma once

#include "../sprites.h"
#include "../../hud/tipsnotification.h"

namespace Impacto {
namespace Profile {
namespace TipsNotification {

inline int TextTableId;
inline int NotificationAlertMessageId;
inline int NotificationTextPart1MessageId;
inline int NotificationTextPart2MessageId;

inline float FadeInDuration;
inline float FadeOutDuration;

void Configure();

void CreateInstance();

}  // namespace TipsNotification
}  // namespace Profile
}  // namespace Impacto