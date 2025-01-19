#pragma once

#include <functional>

#include "../widget.h"

namespace Impacto {
namespace UI {
namespace Widgets {

class ClickArea : public Widget {
 public:
  ClickArea() {}
  ClickArea(int id, RectF bounds);
  ClickArea(int id, RectF bounds,
            std::function<void(ClickArea*)> onClickHandler);

  virtual void UpdateInput() override;

  virtual void Show();
  virtual void Hide();
  virtual void Render() {}

  int Id;

 private:
  bool Clickable = false;
  std::function<void(ClickArea*)> OnClickHandler;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto