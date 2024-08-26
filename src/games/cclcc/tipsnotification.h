#pragma once

#include "../../hud/tipsnotification.h"
#include "../../ui/widgets/label.h"
#include "../../ui/widgets/group.h"

namespace Impacto {
namespace CCLCC {

using namespace Impacto::TipsNotification;

class TipsNotification : public TipsNotificationBase {
 public:
  TipsNotification();

  void Update(float dt);
  void Render();
  void AddTip(int tipId);

 protected:
  Animation Timer;

  UI::Widgets::Label TextPartBefore;
  UI::Widgets::Label TextPartAfter;
  UI::Widgets::Label TipName;

  float PositionY;
};

}  // namespace CCLCC
}  // namespace Impacto