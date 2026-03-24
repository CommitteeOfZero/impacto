#pragma once

#include <queue>

#include <magic_enum/magic_enum.hpp>
#include "../animation.h"

namespace Impacto {
namespace TipsNotification {

enum class TipsNotificationType : int {
  None,
  MO6TW,
  CCLCC,
  CHLCC,
};
class TipsNotificationBase {
 public:
  virtual ~TipsNotificationBase() = default;

  virtual void Update(float dt) = 0;
  virtual void Render() = 0;
  virtual void AddTip(int tipId) = 0;

  Animation FadeAnimation;

 protected:
  std::queue<uint32_t> NotificationQueue;
};

inline std::unique_ptr<TipsNotificationBase> Implementation;

void Init();
void Update(float dt);
void Render();
void AddTip(int tipId);

}  // namespace TipsNotification
}  // namespace Impacto