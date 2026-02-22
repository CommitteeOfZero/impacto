#pragma once

#include "../tipsnotification.h"
#include "../../ui/widgets/label.h"
#include "../../sequencedanimation.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::TipsNotification;

class TipsNotification : public TipsNotificationBase {
 public:
  TipsNotification();

  void Update(float dt) override;
  void Render() override;
  void AddTip(int tipId) override;

 private:
  UI::Widgets::Label Header;

  UI::Widgets::Label TextPartBefore;
  UI::Widgets::Label TextPartAfter;
  UI::Widgets::Label TipName;

  SequencedAnimation TipsAnimation;

  Animation FadeOutAnimation;
  Animation SlideAnimation;
  Animation HoldAnimation;

  bool FirstInQueue = true;
  bool FirstIsRendering = false;
};

}  // namespace CHLCC
}  // namespace Impacto
