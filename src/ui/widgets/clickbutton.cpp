#include "clickbutton.h"

#include "../../inputsystem.h"
#include "../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace Widgets {

ClickButton::ClickButton(int id, RectF bounds,
                         std::function<void(ClickButton*)> onClickHandler)
    : Id(id), OnClickHandler(onClickHandler) {
  Bounds = bounds;
}

void ClickButton::UpdateInput() {
  if (!Enabled) return;

  if (Input::CurrentInputDevice == Input::Device::Mouse &&
      Input::PrevMousePos != Input::CurMousePos) {
    Hovered = Bounds.ContainsPoint(Input::CurMousePos);
  } else if (Input::CurrentInputDevice == Input::Device::Touch &&
             Input::TouchIsDown[0] &&
             Input::PrevTouchPos != Input::CurTouchPos) {
    Hovered = Bounds.ContainsPoint(Input::CurTouchPos);
  }

  if (Hovered && Vm::Interface::PADinputMouseWentDown & Vm::Interface::PAD1A)
    OnClickHandler(this);
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto