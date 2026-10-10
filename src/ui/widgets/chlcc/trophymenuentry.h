#pragma once

#include "../label.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

class TrophyMenuEntry : public Widget {
 public:
  TrophyMenuEntry(int achievementId);

  void Render() override;

  void UpdateInput(float dt) override {};

  using Widget::Move;
  void Move(glm::vec2 relativePosition) override;

 private:
  int AchievementId;

  Label NameLabel;
  Label DescriptionLabel;

  Sprite Icon;
  RectF IconDest;

  Sprite RarityIcon;
  RectF RarityIconDest;
  glm::vec4 RarityIconTint;
};

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto