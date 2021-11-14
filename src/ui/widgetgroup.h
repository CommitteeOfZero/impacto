#pragma once

#include "widget.h"

#include <vector>
#include <enum.h>

namespace Impacto {
namespace UI {

BETTER_ENUM(FocusDirection, int, Vertical = (1 << 0), Horizontal = (1 << 1),
            Both = 3)

class WidgetGroup {
 public:
  WidgetGroup();
  WidgetGroup(glm::vec2 pos);

  std::vector<Widget*> Children;
  void Add(Widget* widget, int focusDirection);
  void AddToFocusChain(Widget* widget, int focusDirection);

  int FocusId = -1;
  std::vector<int> VerticalFocusChain;
  std::vector<int> HorizontalFocusChain;

  glm::vec2 Position;
  RectF RenderingBounds;

  bool HasFocus = false;
  bool IsShown = false;

  void Update(float dt);
  void Render();

  void Show();
  void Hide();

  void Move(glm::vec2 relativePosition, float duration);
  void Move(glm::vec2 relativePosition);
  void MoveTo(glm::vec2 pos, float duration);
  void MoveTo(glm::vec2 pos);

  float Opacity = 1.0f;

 private:
  void UpdateInput();
  void FocusChainAdvance(std::vector<int> const& focusChain, bool forward);
};

}  // namespace UI

}  // namespace Impacto