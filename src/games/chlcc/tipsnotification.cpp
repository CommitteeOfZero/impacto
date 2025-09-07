#include "tipsnotification.h"

#include "../../profile/hud/tipsnotification.h"
#include "../../profile/games/chlcc/tipsnotification.h"
#include "../../data/tipssystem.h"
#include "../../profile/game.h"

namespace Impacto {
namespace CHLCC {

using namespace Impacto::Profile::TipsNotification;
using namespace Impacto::Profile::CHLCC::TipsNotification;
using namespace Impacto::UI::Widgets;

TipsNotification::TipsNotification() {
  const auto headerText =
      Vm::ScriptGetTextTableStrAddress(TextTableId, HeaderMessageId);
  Header = Label(headerText, HeaderPosition, HeaderFontSize,
                 RendererOutlineMode::BottomRight, HeaderColor);
  Header.OutlineAlphaEnabled = true;

  const auto beforeText = Vm::ScriptGetTextTableStrAddress(
      TextTableId, NotificationTextPart1MessageId);
  TextPartBefore = Label(beforeText, TextStartPosition, TextFontSize,
                         RendererOutlineMode::BottomRight, 0);
  TextPartBefore.OutlineAlphaEnabled = true;

  const auto afterText = Vm::ScriptGetTextTableStrAddress(
      TextTableId, NotificationTextPart2MessageId);
  TextPartAfter =
      Label(afterText,
            TextStartPosition + glm::vec2(TextPartBefore.Bounds.Width, 0.0f),
            TextFontSize, RendererOutlineMode::BottomRight, 0);
  TextPartAfter.OutlineAlphaEnabled = true;

  TipName = Label(
      "", TextStartPosition + glm::vec2(TextPartBefore.Bounds.Width, 0.0f),
      TextFontSize, RendererOutlineMode::BottomRight,
      static_cast<int>(TipNameColorIndex));
  TipName.OutlineAlphaEnabled = true;

  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  FadeOutAnimation = FadeAnimation;

  SlideAnimation.DurationIn = SlideTime;

  HoldAnimation.DurationIn = HoldTime;

  TipsAnimation.AddAnimation(FadeAnimation, 0.0f);
  TipsAnimation.AddAnimation(SlideAnimation, 0.0f);
  TipsAnimation.AddAnimation(HoldAnimation);
  TipsAnimation.AddAnimation(FadeOutAnimation, AnimationDirection::Out);
}

void TipsNotification::Update(const float dt) {
  TipsAnimation.Update(dt);

  if (TipsAnimation.State == AnimationState::Stopped &&
      !NotificationQueue.empty()) {
    // Start display animation
    const auto tipNameAdr = NotificationQueue.front();
    const auto tipsScrBufId = TipsSystem::GetTipsScriptBufferId();
    TipName.SetText({.ScriptBufferId = tipsScrBufId, .IpOffset = tipNameAdr},
                    TextFontSize, RendererOutlineMode::BottomRight,
                    static_cast<int>(TipNameColorIndex));
    NotificationQueue.pop();

    TipsAnimation.StartIn(true);

    FirstIsRendering = FirstInQueue;
    FirstInQueue = false;
  }

  if (TipsAnimation.State != AnimationState::Stopped) {
    // Update labels
    const float slideProgress =
        std::sin(SlideAnimation.Progress * std::numbers::pi_v<float> / 2.0f);
    TextPartBefore.MoveTo(TextTargetPosition * slideProgress +
                          TextStartPosition * (1.0f - slideProgress));
    TipName.MoveTo(TextPartBefore.Bounds.GetPos() +
                   glm::vec2(TextPartBefore.Bounds.Width, 0.0f));
    TextPartAfter.MoveTo(TipName.Bounds.GetPos() +
                         glm::vec2(TipName.Bounds.Width, 0.0f));

    // Don't fade out if not the last entry in the queue,
    // and don't fade in if not the first
    float alpha = 1.0f;
    if (FadeOutAnimation.State == AnimationState::Playing) {  // Fading out
      if (NotificationQueue.empty()) alpha = FadeOutAnimation.Progress;
    } else if (FirstIsRendering) {  // Fading in
      alpha = FadeAnimation.Progress;
    }

    Header.Tint.a = alpha;
    TextPartBefore.Tint.a = alpha;
    TipName.Tint.a = alpha;
    TextPartAfter.Tint.a = alpha;

    Header.OutlineAlpha = alpha / 2.0f;
    TextPartBefore.OutlineAlpha = alpha / 2.0f;
    TipName.OutlineAlpha = alpha / 2.0f;
    TextPartAfter.OutlineAlpha = alpha / 2.0f;
  }
}

void TipsNotification::Render() {
  if (TipsAnimation.State == AnimationState::Stopped) return;

  Renderer->EnableScissor();
  Renderer->SetScissorRect(RenderBounds);

  Header.Render();
  TextPartBefore.Render();
  TipName.Render();
  TextPartAfter.Render();

  Renderer->DisableScissor();
}

void TipsNotification::AddTip(const int tipId) {
  FirstInQueue |= TipsAnimation.State == AnimationState::Stopped &&
                  NotificationQueue.empty();

  const auto* const record = TipsSystem::GetTipRecord(tipId);
  NotificationQueue.push(record->StringAdr[1]);
}

}  // namespace CHLCC
}  // namespace Impacto
