#pragma once

#include <enum.h>

#include "../impacto.h"
#include "../util.h"
#include "../animation.h"

namespace Impacto {
namespace UI {

enum FocusDirection { FDIR_LEFT, FDIR_RIGHT, FDIR_UP, FDIR_DOWN };
enum WidgetType { WT_NORMAL, WT_GROUP };

class Widget {
 public:
  bool Enabled = true;
  bool HasFocus = false;
  bool Hovered = false;

  glm::vec4 Tint = glm::vec4(1.0f);

  virtual ~Widget() = default;

  virtual void Update(float dt);
  virtual void UpdateInput(float dt) = 0;
  virtual void Render() = 0;

  virtual void Show();
  virtual void Hide();

  virtual WidgetType GetType();

  // TODO: Text movement in widgets with text
  virtual void Move(glm::vec2 relativePosition) { Bounds += relativePosition; }
  void Move(glm::vec2 relativePosition, float duration);
  void MoveTo(glm::vec2 pos) { Move(pos - Bounds.GetPos()); }
  void MoveTo(glm::vec2 pos, float duration) {
    Move(pos - Bounds.GetPos(), duration);
  }

  virtual Widget* GetFocus(FocusDirection dir);
  virtual void SetFocus(Widget* widget, FocusDirection dir);

  RectF Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  glm::vec2 MoveTarget;
  glm::vec2 MoveOrigin;
  Animation MoveAnimation;

 private:
  Widget* FocusElements[4] = {0, 0, 0, 0};
};

}  // namespace UI

}  // namespace Impacto