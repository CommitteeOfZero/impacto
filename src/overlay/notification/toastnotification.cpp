#include "toastnotification.h"

#include "../overlay.h"
#include "../../profile/overlay.h"
#include "../../profile/game.h"

namespace Impacto::Overlay {

using namespace Profile::Overlay::ToastNotification;

void BuildTextLine(std::vector<ProcessedTextGlyph>& appendBuffer,
                   const std::string_view text, float fontSize,
                   const float left, const float baselineY,
                   const float availableWidth, const uint32_t color) {
  float width = 0.0f;
  std::vector<ExternalFontShapedGlyph> glyphs =
      NotificationFont->ShapeLine(text, fontSize, width);
  if (glyphs.empty()) return;

  if (width > availableWidth && width > 0.0f) {
    fontSize *= availableWidth / width;
    glyphs = NotificationFont->ShapeLine(text, fontSize, width);
  }

  const DialogueColorPair colors{color, TextColors.OutlineColor};

  appendBuffer.reserve(appendBuffer.size() + glyphs.size());
  glm::vec2 pen{left, baselineY};
  for (ExternalFontShapedGlyph const& glyph : glyphs) {
    const glm::vec2 glyphPos = pen + glyph.Offset;

    appendBuffer.push_back({
        .Colors = colors,
        .CharId = glyph.GlyphIndex,
        .Opacity = 1.0f,
        .DestRect = {glyphPos.x, glyphPos.y, glyph.Advance.x, fontSize},
        .Position = pen,
    });

    pen += glyph.Advance;
  }
}

RectF ToastNotificationInterface::GetNotificationRect() {
  const float width = NotificationBackgroundSprite.ScaledWidth() * Scale;
  const float height = NotificationBackgroundSprite.ScaledHeight() * Scale;
  return RectF(Profile::Game::DesignWidth - width,
               Profile::Game::DesignHeight - height, width, height);
}

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
      if (TimeHeld <= DisplayDuration) {
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

TextToastNotification::TextToastNotification(
    const std::span<const std::string_view> lines) {
  const RectF rect = GetNotificationRect();

  const float left = TextPadding * Scale;
  float baselineY = (TextPadding + TextSize) * Scale;
  const float availableWidth = rect.Width - TextPadding * Scale * 2.0f;

  for (const std::string_view line : lines) {
    BuildTextLine(ProcessedText, line, TextSize * Scale, left, baselineY,
                  availableWidth, TextColors.TextColor);
    baselineY += (TextSize + TextLineGap) * Scale;
  }
}

void TextToastNotification::Render(const Animation& animation) {
  RectF rect = GetNotificationRect();
  rect.Y += rect.Height * (1.0f - animation.Progress);

  Renderer->DrawSprite(NotificationBackgroundSprite, rect);

  if (!ProcessedText.empty()) {
    NotificationFont->DrawProcessedText(ProcessedText, 1.0f, 1.0f,
                                        RendererOutlineMode::BottomRight,
                                        rect.GetPos());
  }
}

void TextToastNotification::UpdateInput(float dt, const Animation& animation) {}

}  // namespace Impacto::Overlay
