#pragma once

#include <functional>

#include "../clickarea.h"
#include "../../widget.h"
#include "../../../spritesheet.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

class OptionsEntry : public Widget {
 public:
  OptionsEntry(RectF hoverBounds, std::function<void(OptionsEntry*)> highlight);

  virtual void Update(float dt) override;
  virtual void UpdateInput(float dt) override;

  virtual void Show() override;
  virtual void Hide() override;

  virtual void Move(glm::vec2 relativePos) override;

 protected:
  bool Enabled = false;

  ClickArea EntryButton;
  std::function<void(OptionsEntry*)> Highlight;
  void EntryButtonOnClick(ClickArea* target);
};

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
