#include "tipsnotification.h"

#include "../../profile/scriptvars.h"
#include "../../impacto.h"
#include "../../mem.h"
#include "../../renderer/renderer.h"
#include "../../game.h"
#include "../../profile/hud/tipsnotification.h"
#include "../../profile/games/mo6tw/tipsnotification.h"
#include "../../data/tipssystem.h"
#include "../../ui/menu.h"

namespace Impacto {
namespace MO6TW {

using namespace Impacto::Profile::TipsNotification;
using namespace Impacto::Profile::MO6TW::TipsNotification;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::UI;
using namespace Impacto::UI::Widgets;

TipsNotification::TipsNotification() {
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  Timer.DurationIn = TimerDuration;
  Timer.LoopMode = AnimationLoopMode::Stop;

  AlertTitle = new Group(NULL);
  AlertTitle->FocusLock = true;
  AlertTitle->VisibilityState = Shown;
  AlertTitle->HasFocus = false;

  Notification = new Group(NULL);
  Notification->FocusLock = true;
  Notification->VisibilityState = Shown;
  Notification->HasFocus = false;

  auto textAlert =
      Vm::ScriptGetTextTableStrAddress(TextTableId, NotificationAlertMessageId);
  TextAlert = new Label(textAlert, AlertPosition, FontSize,
                        RendererOutlineMode::Full, AlertTextColorIndex);
  AlertTitle->Add(TextAlert);

  auto textBefore = Vm::ScriptGetTextTableStrAddress(
      TextTableId, NotificationTextPart1MessageId);
  TextPartBefore = new Label(textBefore, FinalNotificationPosition, FontSize,
                             RendererOutlineMode::Full, 0);
  Notification->Add(TextPartBefore);
  auto textAfter = Vm::ScriptGetTextTableStrAddress(
      TextTableId, NotificationTextPart2MessageId);
  TextPartAfter = new Label(
      textAfter,
      glm::vec2(FinalNotificationPosition.x + TextPartBefore->Bounds.Width,
                FinalNotificationPosition.y),
      FontSize, RendererOutlineMode::Full, 0);
  Notification->Add(TextPartAfter);
  TipName = new Label();
  Notification->Add(TipName);
  Notification->Bounds.X = FinalNotificationPosition.x;
  Notification->Bounds.Y = FinalNotificationPosition.y;
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
    auto tipNameAdr = NotificationQueue.front();
    auto tipsScrBufId = TipsSystem::GetTipsScriptBufferId();
    TipName->SetText({.ScriptBufferId = tipsScrBufId, .IpOffset = tipNameAdr},
                     FontSize, RendererOutlineMode::Full, TipNameColorIndex);
    TipName->MoveTo(
        glm::vec2(FinalNotificationPosition.x + TextPartBefore->Bounds.Width,
                  FinalNotificationPosition.y));
    TextPartAfter->MoveTo(glm::vec2(FinalNotificationPosition.x +
                                        TextPartBefore->Bounds.Width +
                                        TipName->Bounds.Width,
                                    FinalNotificationPosition.y));

    NotificationQueue.pop();

    Notification->MoveTo(InitialNotificationPosition);
    Notification->MoveTo(FinalNotificationPosition, MoveAnimationDuration);
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
    AlertTitle->Tint.a = smoothedFade;
    AlertTitle->Render();
    if (Timer.State == AnimationState::Playing ||
        FadeAnimation.Direction == AnimationDirection::Out) {
      Notification->Tint.a = smoothedFade;
      Notification->Render();
    }
  }
}

void TipsNotification::AddTip(int tipId) {
  const auto* const record = TipsSystem::GetTipRecord(tipId);
  NotificationQueue.push(record->StringAdr[0]);
}

}  // namespace MO6TW
}  // namespace Impacto