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
  FadeAnimation.SkipOnSkipMode = false;

  Timer.DurationIn = TimerDuration;
  Timer.LoopMode = AnimationLoopMode::Stop;
  Timer.SkipOnSkipMode = false;

  auto textBefore = Vm::ScriptGetTextTableStrAddress(
      TextTableId, NotificationTextPart1MessageId);
  TextPartBefore =
      Label(textBefore, {NotificationPositionX, 0}, FontSize,
            RendererOutlineMode::BottomRight, NotificationTextTableColorIndex);
  auto textAfter = Vm::ScriptGetTextTableStrAddress(
      TextTableId, NotificationTextPart2MessageId);
  TextPartAfter =
      Label(textAfter, {NotificationPositionX + TextPartBefore.Bounds.Width, 0},
            FontSize, RendererOutlineMode::BottomRight,
            NotificationTextTableColorIndex);
  TipName = Label("", {NotificationPositionX, 0}, FontSize,
                  RendererOutlineMode::BottomRight, TipNameColor);

  TextPartBefore.OutlineAlphaEnabled = true;
  TextPartAfter.OutlineAlphaEnabled = true;
  TipName.OutlineAlphaEnabled = true;
  TextPartBefore.OutlineAlpha = 0.0f;
  TextPartAfter.OutlineAlpha = 0.0f;
  TipName.OutlineAlpha = 0.0f;
}

void TipsNotification::Update(float dt) {
  FadeAnimation.Update(dt);
  Timer.Update(dt);
  PositionY = (float)(static_cast<int>(FadeAnimation.Progress * 255 * 96) >> 8);

  auto UpdateNotificationDisplay = [&]() {
    auto tipNameAdr = NotificationQueue.front();
    auto tipsScrBufId = TipsSystem::GetTipsScriptBufferId();
    TipName.SetText({.ScriptBufferId = tipsScrBufId, .IpOffset = tipNameAdr},
                    FontSize, RendererOutlineMode::BottomRight, TipNameColor);
    Timer.DurationIn = TimerDuration + TipName.GetTextLength() * 0.1f;
    NotificationQueue.pop();
  };
  if (!NotificationQueue.empty()) {
    if (Timer.IsOut()) {
      UpdateNotificationDisplay();
      Timer.StartIn();
    }
    if (FadeAnimation.IsOut()) {
      FadeAnimation.StartIn();
    }
  } else if (FadeAnimation.IsIn() && Timer.IsIn()) {
    FadeAnimation.StartOut();
  }
  if (Timer.IsIn()) Timer.Progress = 0.0f;
}

void TipsNotification::Render() {
  if (FadeAnimation.IsOut()) return;
  if (FadeAnimation.Progress > 0.0f) {
    float smoothedFade = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);
    float bgYPos = PositionY - BackgroundPositionYOffset;
    Renderer->DrawSprite(NotificationBackground, {BackgroundPositionX, bgYPos},
                         glm::vec4(1.0f, 1.0f, 1.0f, smoothedFade));
    TipName.Tint.a = smoothedFade;
    TextPartBefore.Tint.a = smoothedFade;
    TextPartAfter.Tint.a = smoothedFade;
    TipName.OutlineAlpha = smoothedFade / 3.0f;
    TextPartBefore.OutlineAlpha = smoothedFade / 3.0f;
    TextPartAfter.OutlineAlpha = smoothedFade / 3.0f;

    float textYPos = (PositionY + NotificationPositionYOffset);
    TextPartBefore.MoveTo({NotificationPositionX, textYPos});
    float tipNameXPos = NotificationPositionX + TextPartBefore.Bounds.Width;
    TipName.MoveTo({tipNameXPos, textYPos});
    TextPartAfter.MoveTo({tipNameXPos + TipName.Bounds.Width, textYPos});

    TipName.Render();
    TextPartBefore.Render();
    TextPartAfter.Render();
  }
}

void TipsNotification::AddTip(int tipId) {
  const auto* const record = TipsSystem::GetTipRecord(tipId);
  NotificationQueue.push(record->StringAdr[1]);
}

}  // namespace CCLCC
}  // namespace Impacto