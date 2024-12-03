#pragma once

#include "./optionsentry.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class OptionsBinaryButton : public OptionsEntry {
 public:
  OptionsBinaryButton(const Sprite& box, const Sprite& trueLabel,
                      const Sprite& falseLabel, const Sprite& label,
                      glm::vec2 pos, glm::vec4 highlightTint);

  void Render() override;
  void UpdateInput() override;

 private:
  inline glm::vec2 GetTruePos() const;
  inline glm::vec2 GetFalsePos() const;

  const Sprite& BoxSprite;
  const Sprite& TrueSprite;
  const Sprite& FalseSprite;

  bool State = true;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto