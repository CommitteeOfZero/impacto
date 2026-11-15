#include "achievementnotification.h"

#include "toastnotification.h"
#include "../overlay.h"

#include "../../userconfig.h"
#include "../../data/achievementsystem.h"
#include "../../text/fonts/font.h"
#include "../../inputsystem.h"
#include "../../io/physicalfilestream.h"
#include "../../log.h"
#include "../../profile/game.h"
#include "../../profile/overlay.h"
#include "../../renderer/renderer.h"
#include "../../texture/texture.h"

#include <cassert>
#include <string>

namespace Impacto::Overlay {

using namespace Profile::Overlay::ToastNotification;
using namespace Profile::Overlay::ToastNotification::AchievementNotification;

void AchievementNotification::BuildTextGlyphs() {
  TextGlyphs.clear();
  const AchievementSystem::Achievement& achievementData = AchievementData.get();

  std::string title = achievementData.Name();
  std::string description = achievementData.Description();
  if (title.empty() && description.empty()) {
    title = "Unknown Achievement";
    description = "Achievement System not implemented.";
  }

  const RectF rect = GetNotificationRect();
  const float iconRight = (IconOffset.x + IconSize) * Scale;
  const float textLeft = iconRight + TextGap * Scale;
  const float textRight = rect.Width - TextPadding * Scale;
  const float centerY = rect.Height / 2.0f;

  const float titleFontSize = TitleFontSize * Scale;
  const float descriptionFontSize = DescriptionFontSize * Scale;
  const float textLineGap = TextLineGap * Scale;
  const float titleBaseline =
      centerY - (descriptionFontSize + textLineGap) / 2.0f;
  const float descriptionBaseline =
      centerY + (titleFontSize + textLineGap) / 2.0f;
  const float availableWidth = textRight - textLeft;

  Overlay::BuildTextLine(TextGlyphs, title, titleFontSize, textLeft,
                         titleBaseline, availableWidth, TextColors.TextColor);
  Overlay::BuildTextLine(TextGlyphs, description, descriptionFontSize, textLeft,
                         descriptionBaseline, availableWidth,
                         TextColors.TextColor);
}

AchievementNotification::AchievementNotification(
    const AchievementSystem::Achievement& achievement)
    : AchievementData(achievement) {
  BuildTextGlyphs();
}

void AchievementNotification::UpdateInput(const float dt,
                                          const Animation& animation) {
  RectF rect = GetNotificationRect();
  rect.Y += rect.Height * (1.0f - animation.Progress);

  if (rect.ContainsPoint(Input::CurMousePos)) {
    RequestCursor(CursorType::Pointer);
    if (Input::MouseButtonWentDown[SDL_BUTTON_LEFT] ||
        Input::TouchTapCount == 1) {
      Overlay::OverlayShown = true;
      Overlay::RequestedTab = Overlay::OverlayTab::Achievements;
    }
  }
}

void AchievementNotification::Render(const Animation& animation) {
  RectF rect = GetNotificationRect();
  rect.Y += rect.Height * (1.0f - animation.Progress);

  Renderer->DrawSprite(NotificationBackgroundSprite, rect);

  const Sprite& icon = AchievementData.get().Icon();
  if (icon.ScaledWidth() > 0.0f && icon.ScaledHeight() > 0.0f) {
    const RectF iconDest(rect.X + IconOffset.x * Scale,
                         rect.Y + IconOffset.y * Scale, IconSize * Scale,
                         IconSize * Scale);
    Renderer->DrawSprite(icon, iconDest);
  }

  if (!TextGlyphs.empty()) {
    NotificationFont->DrawProcessedText(TextGlyphs, 1.0f, 1.0f,
                                        RendererOutlineMode::BottomRight,
                                        rect.GetPos());
  }
}

void AchievementNotification::Show(const size_t achievementId) {
  if (!magic_enum::enum_flags_test(
          UserConfig::CommonSettings.EnabledNotifications,
          NotificationType::Achievements)) {
    return;
  }

  const AchievementSystem::Achievement* const achievement =
      AchievementSystem::GetAchievement(static_cast<int>(achievementId));

  assert(achievement != nullptr);
  if (achievement != nullptr) {
    Overlay::ToastNotificationHandlerInst.Push(
        std::make_unique<AchievementNotification>(*achievement));
  }
}

}  // namespace Impacto::Overlay
