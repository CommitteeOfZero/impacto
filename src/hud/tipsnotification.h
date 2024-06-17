#pragma once

#include <queue>

#include <enum.h>
#include "../animation.h"

namespace Impacto {
namespace TipsNotification {

BETTER_ENUM(TipsNotificationType, int, None, MO6TW, CCLCC)

class TipsNotificationBase {
 public:
  virtual void Update(float dt) = 0;
  virtual void Render() = 0;
  virtual void AddTip(int tipId) = 0;

  Animation FadeAnimation;

 protected:
  std::queue<uint8_t*> NotificationQueue;
};

inline TipsNotificationBase* Implementation = nullptr;

void Init();
void Update(float dt);
void Render();
void AddTip(int tipId);

}  // namespace TipsNotification
}  // namespace Impacto