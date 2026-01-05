#include "clickarea.h"

#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../renderer/window.h"

namespace Impacto {
namespace UI {
namespace Widgets {

ClickArea::ClickArea(int id, RectF bounds) : Id(id), Clickable(false) {
  Bounds = bounds;
}

ClickArea::ClickArea(int id, RectF bounds,
                     std::function<void(ClickArea*)> onClickHandler)
    : Id(id), Clickable(true), OnClickHandler(onClickHandler) {
  Bounds = bounds;
}

void ClickArea::UpdateInput(float dt) {
  if (!Enabled) return;

  if (Input::CurrentInputDevice == Input::Device::Mouse &&
      ((Input::PrevMousePos != Input::CurMousePos &&
        !(Vm::Interface::PADinputMouseIsDown & Vm::Interface::PAD1A)) ||
       Vm::Interface::PADinputMouseWentDown)) {
    Hovered = Bounds.ContainsPoint(Input::CurMousePos);
  } else if (Input::CurrentInputDevice == Input::Device::Touch &&
             Input::TouchIsDown[0] &&
             Input::PrevTouchPos != Input::CurTouchPos) {
    Hovered = Bounds.ContainsPoint(Input::CurTouchPos);
  }

  if (Clickable && Hovered &&
      Vm::Interface::PADinputMouseWentDown & Vm::Interface::PAD1A) {
    OnClickHandler(this);
  }
}

void ClickArea::Show() {
  Enabled = true;

  switch (Input::CurrentInputDevice) {
    case Input::Device::Mouse:
      Hovered = Bounds.ContainsPoint(Input::CurMousePos);
      break;

    case Input::Device::Touch:
      Hovered = Bounds.ContainsPoint(Input::CurTouchPos);
      break;

    case Input::Device::Keyboard:
    case Input::Device::Controller:
      Hovered = false;
      break;
  }
}

void ClickArea::Hide() {
  Enabled = false;
  Hovered = false;
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto