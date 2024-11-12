#pragma once

#include "../../widget.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class OptionsEntry : public Widget {
 public:
  OptionsEntry(const Sprite& label, glm::vec2 pos, glm::vec4 highlightTint);

  void Render() override;
  void UpdateInput() override;
  void Hide() override;

  bool Selected = false;

 protected:
  const Sprite& LabelSprite;
  glm::vec4 HighlightTint;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto