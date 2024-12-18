#pragma once

#include <functional>

#include "../widget.h"

namespace Impacto {
namespace UI {
namespace Widgets {

class ClickButton : public Widget {
 public:
  ClickButton() {}
  ClickButton(int id, RectF bounds,
              std::function<void(ClickButton*)> onClickHandler);

  virtual void UpdateInput() override;

  int Id;

  std::function<void(ClickButton*)> OnClickHandler;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto