#include "toastnotification.h"

#include "overlay.h"

namespace Impacto::Overlay {

constexpr float HoldTime = 5.0f;

ToastNotificationHandler::ToastNotificationHandler() {
  ShowAnimation.SetDuration(0.5f);
  ShowAnimation.LoopMode = AnimationLoopMode::Stop;
}

void ToastNotificationHandler::Render() {
  if (CurrentNotification == nullptr) return;
  CurrentNotification->Render(ShowAnimation);
}

void ToastNotificationHandler::Update(const float dt) {
  if (OverlayShown) return;

  if (CurrentNotification == nullptr && !PendingNotifications.empty()) {
    assert(ShowAnimation.IsOut());

    CurrentNotification = std::move(PendingNotifications.front());
    PendingNotifications.pop_front();

    ShowAnimation.StartIn();
    TimeHeld = 0.0f;
  }

  if (CurrentNotification != nullptr) {
    CurrentNotification->UpdateInput(dt, ShowAnimation);
    ShowAnimation.Update(dt);

    if (ShowAnimation.IsIn()) {
      if (TimeHeld <= HoldTime) {
        TimeHeld += dt;
      } else {
        ShowAnimation.StartOut();
      }
    } else if (ShowAnimation.IsOut()) {
      CurrentNotification.reset();
    }
  }
}

void ToastNotificationHandler::Push(
    std::unique_ptr<ToastNotificationInterface>&& notification) {
  PendingNotifications.emplace_back(std::move(notification));
}

}  // namespace Impacto::Overlay
