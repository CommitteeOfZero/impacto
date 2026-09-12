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
  using namespace Impacto::Vm::Interface;

  if (!Enabled) return;
  const bool useCursor = (Input::CurrentInputDevice == Input::Device::Mouse ||
                          Input::CurrentInputDevice == Input::Device::Touch);
  if (useCursor && Input::PrevMousePos != Input::CurMousePos) {
    Hovered = Bounds.ContainsPoint(Input::CurMousePos);
  }

  const bool isClicked =
      GetControlState(ControlType::OK, InputDownType::WentDown);
  if (Clickable && isClicked &&
      (!useCursor || (Hovered && Bounds.ContainsPoint(Input::InitMousePos)))) {
    OnClickHandler(this);
  }
}

void ClickArea::Show() {
  Enabled = true;

  switch (Input::CurrentInputDevice) {
    case Input::Device::Mouse:
    case Input::Device::Touch:
      Hovered = Bounds.ContainsPoint(Input::CurMousePos);
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