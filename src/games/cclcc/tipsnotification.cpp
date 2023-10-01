#include "tipsnotification.h"

#include "../../profile/scriptvars.h"
#include "../../impacto.h"
#include "../../mem.h"
#include "../../renderer/renderer.h"
#include "../../game.h"
#include "../../profile/hud/tipsnotification.h"
#include "../../profile/games/cclcc/tipsnotification.h"
#include "../../data/tipssystem.h"

namespace Impacto {
namespace CCLCC {

using namespace Impacto::Profile::TipsNotification;
using namespace Impacto::Profile::CCLCC::TipsNotification;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::UI::Widgets;

TipsNotification::TipsNotification() {
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  Timer.DurationIn = TimerDuration;
  Timer.LoopMode = ALM_Stop;

  AlertTitle = new Group(NULL);
  AlertTitle->FocusLock = true;
  AlertTitle->IsShown = true;
  AlertTitle->HasFocus = false;

  Notification = new Group(NULL);
  Notification->FocusLock = true;
  Notification->IsShown = true;
  Notification->HasFocus = false;

  auto textBefore = Vm::ScriptGetTextTableStrAddress(
      TextTableId, NotificationTextPart1MessageId);
  TextPartBefore =
      new Label(textBefore, InitialNotificationPosition, FontSize, RO_Full, 0);
  Notification->Add(TextPartBefore);
  auto textAfter = Vm::ScriptGetTextTableStrAddress(
      TextTableId, NotificationTextPart2MessageId);
  TextPartAfter = new Label(
      textAfter,
      glm::vec2(InitialNotificationPosition.x + TextPartBefore->Bounds.Width,
                InitialNotificationPosition.y),
      FontSize, RO_Full, 0);
  Notification->Add(TextPartAfter);
  TipName = new Label();
  Notification->Add(TipName);
  Notification->Bounds.X = InitialNotificationPosition.x;
  Notification->Bounds.Y = InitialNotificationPosition.y;
  Notification->RenderingBounds = NotificationRenderingBounds;
}

void TipsNotification::Update(float dt) {
  FadeAnimation.Update(dt);
  Timer.Update(dt);
  Notification->Update(dt);
  if (!NotificationQueue.empty() && !FadeAnimation.IsIn()) {
    FadeAnimation.StartIn();
  }
  if (FadeAnimation.IsIn() && Timer.IsOut()) {
    Timer.StartIn();
    auto tipName = NotificationQueue.front();
    TipName->SetText(tipName, FontSize, RO_Full, TipNameColorIndex);
    NotificationQueue.pop();
  }
  if (NotificationQueue.empty() && FadeAnimation.IsIn() && Timer.IsIn()) {
    FadeAnimation.StartOut();
  }
  if (Timer.IsIn()) Timer.Progress = 0.0f;
}

void TipsNotification::Render() {
  if (FadeAnimation.IsOut()) return;
  if (FadeAnimation.Progress > 0.0f) {
    float smoothedFade = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);
    Renderer->DrawSprite(NotificationBackground, BackgroundPosition,
                         glm::vec4(1.0f, 1.0f, 1.0f, smoothedFade));
    if (Timer.State == AS_Playing || FadeAnimation.Direction == -1) {
      Notification->Tint.a = smoothedFade;
      Notification->Render();
    }
  }
}

void TipsNotification::AddTip(int tipId) {
  auto record = &TipsSystem::GetTipRecords()[tipId];
  NotificationQueue.push(record->StringPtrs[0]);
}

}  // namespace CCLCC
}  // namespace Impacto