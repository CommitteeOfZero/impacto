#pragma once

#include "../tipsnotification.h"
#include "../../ui/widgets/label.h"
#include "../../ui/widgets/group.h"

namespace Impacto {
namespace MO6TW {

using namespace Impacto::TipsNotification;

class TipsNotification : public TipsNotificationBase {
 public:
  TipsNotification();

  void Update(float dt);
  void Render();
  void AddTip(int tipId);

 protected:
  Animation Timer;

  UI::Widgets::Group* AlertTitle;
  UI::Widgets::Group* Notification;

  UI::Widgets::Label* TextAlert;
  UI::Widgets::Label* TextPartBefore;
  UI::Widgets::Label* TextPartAfter;
  UI::Widgets::Label* TipName;
};

}  // namespace MO6TW
}  // namespace Impacto