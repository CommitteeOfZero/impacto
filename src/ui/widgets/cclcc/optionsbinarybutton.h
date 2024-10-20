#pragma once

#include "../../widget.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class OptionsBinaryButton : public Widget {
 public:
  OptionsBinaryButton(const Sprite& box, const Sprite& trueLabel,
                      const Sprite& falseLabel, const Sprite& label,
                      glm::vec2 pos);

  void Render() override;
  void UpdateInput() override;

 private:
  inline glm::vec2 GetTruePos() const;
  inline glm::vec2 GetFalsePos() const;

  const Sprite& BoxSprite;
  const Sprite& TrueSprite;
  const Sprite& FalseSprite;
  const Sprite& LabelSprite;

  glm::vec4 HighlightTint = glm::vec4(0.94f, 0.49f, 0.59f, 1.0f);

  bool State = true;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto