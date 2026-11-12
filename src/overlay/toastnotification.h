#pragma once

#include "../animation.h"

#include <deque>
#include <memory>

namespace Impacto::Overlay {

struct ToastNotificationInterface {
  virtual void Render(const Animation& animation) = 0;
  virtual void UpdateInput(float dt, const Animation& animation) {}
};

class ToastNotificationHandler {
 public:
  ToastNotificationHandler();

  void Render();
  void Update(float dt);

  void Push(std::unique_ptr<ToastNotificationInterface>&& notification);

 private:
  std::unique_ptr<ToastNotificationInterface> CurrentNotification;
  std::deque<std::unique_ptr<ToastNotificationInterface>> PendingNotifications;

  Animation ShowAnimation;
  float TimeHeld = 0.0f;
};

inline ToastNotificationHandler ToastNotificationHandlerInst;

}  // namespace Impacto::Overlay
