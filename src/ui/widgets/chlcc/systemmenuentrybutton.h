#pragma once

#include "../button.h"
#include "../../../animation.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

class SystemMenuEntryButton : public Button {
 public:
  SystemMenuEntryButton(int id, Sprite const& norm, Sprite const& focused,
                        glm::vec4 focusTint, Sprite const& highlight,
                        glm::vec2 pos, RectF hoverBounds);
  void Render() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;

 private:
  glm::vec2 RotatePoint(const glm::vec2& point, const glm::vec2& center,
                        float angleRadians);

 protected:
  Animation StarAnimation;
  glm::vec4 FocusTint;
};

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto