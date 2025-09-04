#include "widget.h"

namespace Impacto {
namespace UI {
void Widget::Update(float dt) {
  if (MoveAnimation.State == AnimationState::Playing) {
    MoveAnimation.Update(dt);
    auto move = glm::mix(MoveOrigin, MoveTarget, MoveAnimation.Progress);
    MoveTo(move);
  }
}

void Widget::Show() {}
void Widget::Hide() {
  HasFocus = false;
  Hovered = false;
}

WidgetType Widget::GetType() { return WT_NORMAL; }

void Widget::Move(glm::vec2 relativePosition, float duration) {
  MoveOrigin = Bounds.GetPos();
  MoveTarget = MoveOrigin + relativePosition;
  MoveAnimation.Progress = 0.0f;
  MoveAnimation.Direction = AnimationDirection::In;
  MoveAnimation.SetDuration(duration);
  MoveAnimation.StartIn();
}

void Widget::Move(glm::vec2 relativePosition) {
  Bounds.X += relativePosition.x;
  Bounds.Y += relativePosition.y;
}

void Widget::MoveTo(glm::vec2 pos, float duration) {
  MoveOrigin = Bounds.GetPos();
  MoveTarget = pos;
  MoveAnimation.Progress = 0.0f;
  MoveAnimation.Direction = AnimationDirection::In;
  MoveAnimation.SetDuration(duration);
  MoveAnimation.StartIn();
}

Widget* Widget::GetFocus(FocusDirection dir) {
  Widget* nextFocus = FocusElements[dir];
  while (nextFocus && !nextFocus->Enabled) {
    nextFocus = nextFocus->FocusElements[dir];
  }
  return nextFocus;
}

void Widget::SetFocus(Widget* widget, FocusDirection dir) {
  FocusElements[dir] = widget;
}

}  // namespace UI
}  // namespace Impacto