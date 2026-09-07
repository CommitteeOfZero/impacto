#include "toggle.h"
#include "../../vm/thread.h"
#include "../../profile/dialogue.h"
#include "../../renderer/renderer.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"

namespace Impacto {
namespace UI {
namespace Widgets {

Toggle::Toggle(int id, bool* value, Sprite const& enabled,
               Sprite const& disabled, Sprite const& highlight, glm::vec2 pos,
               bool isCheckbox) {
  Id = id;
  Value = value;
  EnabledSprite = enabled;
  DisabledSprite = disabled;
  HighlightSprite = highlight;
  IsCheckbox = isCheckbox;
  Bounds = RectF(pos.x, pos.y, EnabledSprite.ScaledWidth(),
                 EnabledSprite.ScaledHeight());
}

Toggle::Toggle(int id, bool* value, Sprite const& enabled,
               Sprite const& disabled, Sprite const& highlight, glm::vec2 pos,
               bool isCheckbox, Vm::Sc3Stream& stream, glm::vec2 labelOfs,
               float fontSize, RendererOutlineMode outlineMode)
    : Toggle(id, value, enabled, disabled, highlight, pos, isCheckbox) {
  HasTextLabel = true;
  SetText(stream, fontSize, outlineMode);
}

Toggle::Toggle(int id, bool* value, Sprite const& enabled,
               Sprite const& disabled, Sprite const& highlight, glm::vec2 pos,
               bool isCheckbox, Sprite const& label, glm::vec2 labelOfs)
    : Toggle(id, value, enabled, disabled, highlight, pos, isCheckbox) {
  HasSpriteLabel = true;
  LabelSprite = label;
  LabelOffset = labelOfs;
}

void Toggle::UpdateInput(float dt) {
  using namespace Impacto::Vm::Interface;

  if (!Enabled) return;
  const bool useCursor = (Input::CurrentInputDevice == Input::Device::Mouse ||
                          Input::CurrentInputDevice == Input::Device::Touch);
  if (useCursor && Input::PrevMousePos != Input::CurMousePos) {
    Hovered = Bounds.ContainsPoint(Input::CurMousePos);
  }
  const bool isClicked =
      GetControlState(ControlType::OK, InputDownType::WentDown);
  if (HasFocus && isClicked &&
      (!useCursor || (Hovered && Bounds.ContainsPoint(Input::InitMousePos)))) {
    *Value = !*Value;
    if (OnClickHandler) OnClickHandler(this);
  }
}

void Toggle::Update(float dt) { Widget::Update(dt); }

void Toggle::Render() {
  if (!*Value || IsCheckbox) {
    Renderer->DrawSprite(DisabledSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  }
  if (*Value) {
    Renderer->DrawSprite(EnabledSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  }
  if (HasFocus) {
    auto tint = Tint;
    if (IsCheckbox) tint.a *= 0.5f;
    Renderer->DrawSprite(HighlightSprite, glm::vec2(Bounds.X, Bounds.Y), tint);
  }
  if (HasSpriteLabel) {
    Renderer->DrawSprite(LabelSprite,
                         glm::vec2(Bounds.X, Bounds.Y) + LabelOffset, Tint);
  }
  if (HasTextLabel) {
    Profile::Dialogue::DialogueFont->DrawProcessedText(Label, Tint.a,
                                                       OutlineMode);
  }
}

void Toggle::SetText(Vm::Sc3Stream& stream, float fontSize,
                     RendererOutlineMode outlineMode) {
  Label = TextLayoutPlainLine(stream, 255, *Profile::Dialogue::DialogueFont,
                              fontSize, Profile::Dialogue::ColorTable[10], 1.0f,
                              glm::vec2(Bounds.X, Bounds.Y) + LabelOffset,
                              TextAlignment::Left);
  OutlineMode = outlineMode;
  FontSize = fontSize;
  TextWidth = GetTextWidth(Label);
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto