#pragma once

#include <functional>

#include "../clickarea.h"
#include "../../widget.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

class OptionsEntry : public Widget {
 public:
  OptionsEntry(const Sprite& label, glm::vec2 pos, glm::vec4 highlightTint,
               std::function<void(OptionsEntry*)> select,
               std::function<void(Widget*)> highlight);

  void Render() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;

  void Show() override;
  void Hide() override;

  using Widget::Move;
  void Move(glm::vec2 relativePos) override;

  bool Selected = false;

 protected:
  ClickArea EntryButton;
  std::function<void(OptionsEntry*)> Select;
  std::function<void(OptionsEntry*)> Highlight;
  void EntryButtonOnClick(ClickArea* target);

  const Sprite& LabelSprite;
  glm::vec4 HighlightTint;
};

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto