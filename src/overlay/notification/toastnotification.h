#pragma once

#include "../overlay.h"
#include "../../animation.h"
#include "../../spritesheet.h"
#include "../../text/processedtextglyph.h"
#include "../../text/fonts/font.h"

#include <optional>
#include <string_view>
#include <vector>
#include <deque>
#include <memory>

namespace Impacto::Overlay {

enum class NotificationType : uint8_t {
  None = 0,
  Hints = 1 << 0,
  Updates = 1 << 1,
  Achievements = 1 << 2,
  All = (Achievements << 1) - 1,
};

void BuildTextLine(std::vector<ProcessedTextGlyph>& appendBuffer,
                   std::string_view text, float fontSize, float left,
                   float baselineY, float availableWidth, uint32_t color);

inline std::vector<ProcessedTextGlyph> BuildTextLine(std::string_view text,
                                                     float fontSize, float left,
                                                     float baselineY,
                                                     float availableWidth,
                                                     uint32_t color) {
  std::vector<ProcessedTextGlyph> buffer;
  BuildTextLine(buffer, text, fontSize, left, baselineY, availableWidth, color);
  return buffer;
}

struct ToastNotificationInterface {
  virtual ~ToastNotificationInterface() = default;

  virtual void Render(const Animation& animation) = 0;
  virtual void UpdateInput(float dt, const Animation& animation) {}

  [[nodiscard]] static RectF GetNotificationRect();
};

struct TextToastNotification final : public ToastNotificationInterface {
 public:
  TextToastNotification(std::span<const std::string_view> lines,
                        std::function<void()> OnClick = nullptr);
  TextToastNotification(std::string_view text,
                        std::function<void()> onClick = nullptr)
      : TextToastNotification(std::array<const std::string_view, 1>{text},
                              onClick) {}

  TextToastNotification(std::span<const std::string_view> lines,
                        OverlayTab openTabOnClick);
  TextToastNotification(std::string_view text, OverlayTab openTabOnClick)
      : TextToastNotification(std::array<const std::string_view, 1>{text},
                              openTabOnClick) {}

  void Render(const Animation& animation) override;
  void UpdateInput(float dt, const Animation& animation) override;

 private:
  std::vector<ProcessedTextGlyph> ProcessedText;
  std::function<void()> OnClick;
};

class ToastNotificationHandler {
 public:
  ToastNotificationHandler();

  void Render();
  void Update(float dt);

  void Push(std::unique_ptr<ToastNotificationInterface>&& notification);

 private:
  std::unique_ptr<ToastNotificationInterface> CurrentNotification;
  std::deque<std::unique_ptr<ToastNotificationInterface>> PendingNotifications;

  Animation ShowAnimation;
  float TimeHeld = 0.0f;
};

inline ToastNotificationHandler ToastNotificationHandlerInst;

}  // namespace Impacto::Overlay

template <>
struct magic_enum::customize::enum_range<Impacto::Overlay::NotificationType> {
  static constexpr bool is_flags = true;
};
