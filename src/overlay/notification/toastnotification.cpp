#include "toastnotification.h"

#include "../overlay.h"
#include "../../inputsystem.h"
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
  const float scale = GetScale();

  const float width = NotificationBackgroundSprite.ScaledWidth() * scale;
  const float height = NotificationBackgroundSprite.ScaledHeight() * scale;
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
    const std::span<const std::string_view> lines,
    std::function<void()> onClick)
    : OnClick(onClick) {
  const RectF rect = GetNotificationRect();
  ProcessedTextScale = GetScale();

  const float availableWidth =
      rect.Width - TextPadding * ProcessedTextScale * 2.0f;

  float baselineY = 0.0f;
  for (const std::string_view line : lines) {
    BuildTextLine(ProcessedText, line, TextSize * ProcessedTextScale, 0.0f,
                  baselineY, availableWidth, TextColors.TextColor);
    baselineY += (TextSize + TextLineGap) * ProcessedTextScale;
  }
}

TextToastNotification::TextToastNotification(
    const std::span<const std::string_view> lines, OverlayTab openTabOnClick)
    : TextToastNotification(lines, [openTabOnClick]() {
        Overlay::OverlayShown = true;
        Overlay::RequestedTab = openTabOnClick;
      }) {}

void TextToastNotification::Render(const Animation& animation) {
  RectF rect = GetNotificationRect();
  rect.Y += rect.Height * (1.0f - animation.Progress);

  Renderer->DrawSprite(NotificationBackgroundSprite, rect);

  if (!ProcessedText.empty()) {
    const float scale = GetScale();
    const glm::vec2 offset = {TextPadding * scale,
                              (TextPadding + TextSize) * scale};
    const glm::mat4 transformation = TransformationMatrix(
        glm::vec2(0.0f), glm::vec2(GetScale() / ProcessedTextScale),
        glm::vec2(0.0f), 0.0f, rect.GetPos() + offset);

    NotificationFont->DrawProcessedText(ProcessedText, 1.0f,
                                        RendererOutlineMode::BottomRight,
                                        nullptr, transformation);
  }
}

void TextToastNotification::UpdateInput(float dt, const Animation& animation) {
  if (OnClick == nullptr) return;

  RectF rect = GetNotificationRect();
  rect.Y += rect.Height * (1.0f - animation.Progress);

  if (rect.ContainsPoint(Input::CurMousePos)) {
    RequestCursor(CursorType::Pointer);
    if (Input::MouseButtonWentDown[SDL_BUTTON_LEFT] ||
        Input::TouchTapCount == 1) {
      OnClick();
    }
  }
}

}  // namespace Impacto::Overlay
