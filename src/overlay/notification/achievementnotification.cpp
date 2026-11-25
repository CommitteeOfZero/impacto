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
#include "../../profile/overlay/overlay.h"
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
  TextGlyphsScale = GetScale();

  const float iconRight = (IconOffset.x + IconSize) * TextGlyphsScale;

  const float titleFontSize = TitleFontSize * TextGlyphsScale;
  const float descriptionFontSize = DescriptionFontSize * TextGlyphsScale;
  const float descriptionBaseline =
      (titleFontSize + descriptionFontSize) / 2.0f +
      TextLineGap * TextGlyphsScale;
  const float availableWidth =
      rect.Width - iconRight - (TextPadding + TextGap) * TextGlyphsScale;

  Overlay::BuildTextLine(TextGlyphs, title, titleFontSize, 0.0f, 0.0f,
                         availableWidth, TextColors.TextColor);
  Overlay::BuildTextLine(TextGlyphs, description, descriptionFontSize, 0.0f,
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
  const float scale = GetScale();

  Renderer->DrawSprite(NotificationBackgroundSprite, rect);

  const Sprite& icon = AchievementData.get().Icon();
  const RectF iconDest(rect.X + IconOffset.x * scale,
                       rect.Y + IconOffset.y * scale, IconSize * scale,
                       IconSize * scale);
  Renderer->DrawSprite(icon, iconDest);

  if (!TextGlyphs.empty()) {
    const glm::vec2 offset = {
        iconDest.Right() + TextGap * scale,
        rect.Top() +
            (rect.Height - (DescriptionFontSize + TextLineGap) * scale) / 2.0f};
    const glm::mat4 transformation = TransformationMatrix(
        glm::vec2(0.0f), glm::vec2(scale / TextGlyphsScale), glm::vec2(0.0f),
        0.0f, offset);

    NotificationFont->DrawProcessedText(TextGlyphs, 1.0f,
                                        RendererOutlineMode::BottomRight,
                                        nullptr, transformation);
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
