#pragma once

#include "toastnotification.h"
#include "../../data/achievementsystem.h"
#include "../../text/processedtextglyph.h"

namespace Impacto::Overlay {

struct AchievementNotification final : public ToastNotificationInterface {
 public:
  AchievementNotification(const AchievementSystem::Achievement& achievement);

  void Render(const Animation& animation) override;
  void UpdateInput(float dt, const Animation& animation) override;

  static void Show(size_t achievementId);

 private:
  std::reference_wrapper<const AchievementSystem::Achievement> AchievementData;
  std::vector<ProcessedTextGlyph> TextGlyphs;

  void BuildTextGlyphs();
};

}  // namespace Impacto::Overlay
