#pragma once

#include "../../widget.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class OptionsSlider : public Widget {
 public:
  OptionsSlider(const Sprite& box, const Sprite& label, glm::vec2 pos,
                glm::vec4 highlightTint);

  void Render() override;
  void UpdateInput() override;

 private:
  const Sprite& BoxSprite;
  const Sprite& LabelSprite;

  glm::vec4 HighlightTint;

  float Progress = 0.0f;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto