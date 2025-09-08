#pragma once

#include "../../ui/widget.h"
#include "../../ui/widgets/label.h"
#include "../../spritesheet.h"
#include "../../profile/games/chlcc/trophymenu.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

class TrophyMenuEntry : public Widget {
 public:
  TrophyMenuEntry(int achievementId);
  void Update(float dt) override;
  void UpdateInput(float dt) override {};
  void Render() override;
  void Move(glm::vec2 relativePosition) override;
  void UpdateOffset(glm::vec2 offset);

 private:
  int AchievementId;
  Impacto::UI::Widgets::Label NameLabel;
  Impacto::UI::Widgets::Label DescriptionLabel;
  Impacto::Sprite Icon;
  RectF iconDest;
  glm::vec2 Offset;
};
}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto