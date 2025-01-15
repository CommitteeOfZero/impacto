#include "optionsentry.h"

#include "../../../profile/games/cclcc/optionsmenu.h"
#include "../../../renderer/renderer.h"
#include "../../../vm/interface/input.h"

using namespace Impacto::Profile::CCLCC::OptionsMenu;
using namespace Impacto::Vm::Interface;

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

OptionsEntry::OptionsEntry(const Sprite& label, glm::vec2 pos,
                           glm::vec4 highlightTint,
                           std::function<void(OptionsEntry*)> select,
                           std::function<void(Widget*)> highlight)
    : LabelSprite(label),
      HighlightTint(highlightTint),
      Select(select),
      Highlight(highlight) {
  Bounds = RectF(pos.x, pos.y, LabelOffset.x + LabelSprite.ScaledWidth(),
                 LabelOffset.y + LabelSprite.ScaledHeight());

  std::function<void(ClickButton*)> onClick =
      std::bind(&OptionsEntry::EntryButtonOnClick, this, std::placeholders::_1);
  EntryButton = ClickButton(0, Bounds, onClick);
}

void OptionsEntry::Render() {
  HighlightTint.a = Tint.a;

  if (HasFocus) {
    RectF highlightBoundBox(Bounds.X, Bounds.Y, EntryDimensions.x,
                            EntryDimensions.y);
    Renderer->DrawRect(highlightBoundBox, HighlightTint);
    Renderer->DrawRect(
        RectF(highlightBoundBox.X + 2.0f, highlightBoundBox.Y + 2.0f,
              highlightBoundBox.Width - 4.0f, highlightBoundBox.Height - 4.0f),
        glm::vec4(1.0f, 1.0f, 1.0f, Tint.a));

    Renderer->DrawSprite(PointerSprite, Bounds.GetPos() + PointerOffset, Tint);
  }

  Renderer->DrawSprite(LabelSprite, Bounds.GetPos() + LabelOffset,
                       Selected ? Tint : glm::vec4(0.0f, 0.0f, 0.0f, Tint.a));
}

void OptionsEntry::Update(float dt) {
  Widget::Update(dt);

  EntryButton.Update(dt);
}

void OptionsEntry::UpdateInput() {
  const bool wasHovered = EntryButton.Hovered;
  EntryButton.UpdateInput();
  if (!HasFocus && !wasHovered && EntryButton.Hovered) {
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0.0f);
    Highlight(this);
  }

  if (!HasFocus) return;

  if (PADinputButtonWentDown & PAD1A) {
    Selected = !Selected;
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 2, false, 0.0f);
    return;
  }

  if (Selected && PADinputButtonWentDown & PAD1B) {
    Selected = false;
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 3, false, 0.0f);
    return;
  }
}

void OptionsEntry::Show() { EntryButton.Show(); }

void OptionsEntry::Hide() {
  Widget::Hide();
  EntryButton.Hide();
  Selected = false;
}

void OptionsEntry::Move(glm::vec2 relativePos) {
  Widget::Move(relativePos);
  EntryButton.Move(relativePos);
}

void OptionsEntry::MoveTo(glm::vec2 pos) {
  const glm::vec2 relativePosition = pos - Bounds.GetPos();
  Widget::MoveTo(pos);
  EntryButton.Move(relativePosition);
}

void OptionsEntry::EntryButtonOnClick(ClickButton* target) {
  if (Selected) return;

  Audio::Channels[Audio::AC_SSE]->Play("sysse", 2, false, 0.0f);
  Select(this);
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto