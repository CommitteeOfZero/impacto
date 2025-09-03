#include "optionsentry.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

OptionsEntry::OptionsEntry(const RectF hoverBounds,
                           const std::function<void(OptionsEntry*)> highlight)
    : EntryButton(0, hoverBounds), Highlight(highlight) {
  Bounds = hoverBounds;
}

void OptionsEntry::Update(float dt) {
  Widget::Update(dt);

  EntryButton.Update(dt);
}

void OptionsEntry::UpdateInput(float dt) {
  const bool wasHovered = EntryButton.Hovered;
  EntryButton.UpdateInput(dt);
  if (!HasFocus && !wasHovered && EntryButton.Hovered) {
    Highlight(this);
  }
}

void OptionsEntry::Show() {
  Widget::Show();
  EntryButton.Show();
}

void OptionsEntry::Hide() {
  EntryButton.Hide();
  Widget::Hide();
}

void OptionsEntry::Move(glm::vec2 relativePos) {
  Widget::Move(relativePos);
  EntryButton.Move(relativePos);
}

void OptionsEntry::MoveTo(glm::vec2 pos) {
  Widget::MoveTo(pos);
  EntryButton.MoveTo(pos);
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
