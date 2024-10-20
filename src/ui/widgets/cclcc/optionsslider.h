#pragma once

#include "../../widget.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class OptionsSlider : public Widget {
 public:
  OptionsSlider(const Sprite& box, const Sprite& label, glm::vec2 pos);

  void Render() override;
  void UpdateInput() override;

 private:
  const Sprite& BoxSprite;
  const Sprite& LabelSprite;

  glm::vec4 HighlightTint = glm::vec4(0.94f, 0.49f, 0.59f, 1.0f);

  float Progress = 0.0f;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto