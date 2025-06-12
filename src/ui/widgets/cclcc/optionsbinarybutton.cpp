#include "optionsbinarybutton.h"

#include "../../../profile/games/cclcc/optionsmenu.h"
#include "../../../renderer/renderer.h"
#include "../../../vm/interface/input.h"
#include "../../../inputsystem.h"

using namespace Impacto::Profile::CCLCC::OptionsMenu;
using namespace Impacto::Vm::Interface;

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {

OptionsBinaryButton::OptionsBinaryButton(
    bool& value, const Sprite& box, const Sprite& trueLabel,
    const Sprite& falseLabel, const Sprite& label, glm::vec2 pos,
    glm::vec4 highlightTint, std::function<void(OptionsEntry*)> select,
    std::function<void(Widget*)> highlight)
    : OptionsEntry(label, pos, highlightTint, select, highlight),
      State(value),
      BoxSprite(box),
      TrueSprite(trueLabel),
      FalseSprite(falseLabel) {
  Bounds.Width = BinaryBoxOffset.x + BoxSprite.ScaledWidth();
  EntryButton.Bounds.Width = Bounds.Width;

  glm::vec2 truePosition = pos + BinaryBoxOffset;
  TrueButton =
      ClickArea(0,
                RectF(truePosition.x, truePosition.y, TrueSprite.ScaledWidth(),
                      TrueSprite.ScaledHeight()),
                [this](auto* btn) { return TrueOnClick(btn); });
  FalseButton = ClickArea(
      0, TrueButton.Bounds + glm::vec2(box.ScaledWidth() / 2.0f, 0.0f),
      [this](auto* btn) { return FalseOnClick(btn); });
}

void OptionsBinaryButton::Render() {
  OptionsEntry::Render();

  RectF highlightBounds(0.0f, 0.0f, BoxSprite.ScaledWidth() / 2,
                        BoxSprite.ScaledHeight());
  glm::vec2 highlightPos = ((State) ? TrueButton : FalseButton).Bounds.GetPos();
  highlightBounds.X = highlightPos.x;
  highlightBounds.Y = highlightPos.y;

  Renderer->DrawQuad(highlightBounds, HighlightTint);
  Renderer->DrawSprite(BoxSprite, TrueButton.Bounds.GetPos(), Tint);

  Renderer->DrawSprite(TrueSprite, TrueButton.Bounds.GetPos(), Tint, !State);
  Renderer->DrawSprite(FalseSprite, FalseButton.Bounds.GetPos(), Tint, State);
}

void OptionsBinaryButton::Update(float dt) {
  OptionsEntry::Update(dt);

  TrueButton.Update(dt);
  FalseButton.Update(dt);
}

void OptionsBinaryButton::UpdateInput() {
  // Handle mouse/touch input
  TrueButton.UpdateInput();
  FalseButton.UpdateInput();

  OptionsEntry::UpdateInput();

  if (!Selected) return;

  // Handle keyboard/controller input
  if (PADinputButtonWentDown & (PAD1LEFT | PAD1RIGHT)) {
    const bool newState = PADinputButtonWentDown & PAD1LEFT;

    if (State != newState)
      Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0.0f);

    State = newState;
  }
}

void OptionsBinaryButton::Show() {
  OptionsEntry::Show();

  TrueButton.Show();
  FalseButton.Show();
}

void OptionsBinaryButton::Hide() {
  OptionsEntry::Hide();

  TrueButton.Hide();
  FalseButton.Hide();
}

void OptionsBinaryButton::Move(glm::vec2 relativePos) {
  OptionsEntry::Move(relativePos);
  TrueButton.Move(relativePos);
  FalseButton.Move(relativePos);
}

void OptionsBinaryButton::MoveTo(glm::vec2 pos) {
  const glm::vec2 relativePosition = pos - Bounds.GetPos();
  OptionsEntry::MoveTo(pos);
  TrueButton.Move(relativePosition);
  FalseButton.Move(relativePosition);
}

void OptionsBinaryButton::TrueOnClick(ClickArea* target) {
  if (Selected && State != true)
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0.0f);

  State = true;
}

void OptionsBinaryButton::FalseOnClick(ClickArea* target) {
  if (Selected && State != false)
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0.0f);

  State = false;
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto