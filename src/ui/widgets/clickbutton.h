#pragma once

#include <functional>

#include "../widget.h"

namespace Impacto {
namespace UI {
namespace Widgets {

class ClickButton : public Widget {
 public:
  ClickButton() {}
  ClickButton(int id, RectF bounds);
  ClickButton(int id, RectF bounds,
              std::function<void(ClickButton*)> onClickHandler);

  virtual void UpdateInput() override;

  virtual void Show();
  virtual void Hide();
  virtual void Render() {}

  int Id;

 private:
  bool Clickable = false;
  std::function<void(ClickButton*)> OnClickHandler;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto