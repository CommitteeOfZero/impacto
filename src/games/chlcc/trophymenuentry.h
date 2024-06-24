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
  TrophyMenuEntry(int achievementId, float heightOffset);
  void Update(float dt) override;
  void UpdateInput() {};
  void Render() override;
  void Move(glm::vec2 relativePosition) override;
  void MoveTo(glm::vec2 pos) override;

 private:
  int AchievementId;
  glm::vec2 Position;
  Impacto::UI::Widgets::Label NameLabel;
  Impacto::UI::Widgets::Label DescriptionLabel;
  Impacto::Sprite Icon;
  RectF iconDest;
};
}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto