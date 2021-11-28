#pragma once

#include "../widget.h"
#include "../menu.h"

#include <vector>
#include <enum.h>

namespace Impacto {
namespace UI {
namespace Widgets {

class Group : public Widget {
 public:
  Group(Menu* ctx);
  Group(Menu* ctx, glm::vec2 pos);

  std::vector<Widget*> Children;
  void Add(Widget* widget);
  void Add(Widget* widget, FocusDirection dir);

  glm::vec2 Position;
  RectF RenderingBounds;

  bool IsShown = false;
  bool FocusLock = true;
  bool WrapFocus = true;

  void Update(float dt) override;
  void Render() override;
  void UpdateInput() override;

  Widget* GetFocus(FocusDirection dir) override;

  void Show() override;
  void Hide() override;

  WidgetType GetType() override;

  void Clear();

  void Move(glm::vec2 relativePosition, float duration) override;
  void Move(glm::vec2 relativePosition) override;
  void MoveTo(glm::vec2 pos, float duration) override;
  void MoveTo(glm::vec2 pos) override;

 private:
  Menu* MenuContext;
  int FirstFocusableElementId = -1;
  int LastFocusableElementId = -1;
  Widget* FocusStart[4] = {0, 0, 0, 0};
  Widget* PreviousFocusElement = 0;
  Widget* PreviousFocusStart[4] = {0, 0, 0, 0};
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto