#include "button.h"
#include "../../renderer/renderer.h"
#include "../../inputsystem.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../vm/interface/input.h"
#include "../../vm/thread.h"
#include "../../profile/dialogue.h"

namespace Impacto {
namespace UI {
namespace Widgets {

using namespace Impacto::Profile::ScriptVars;

Button::Button(int id, std::optional<Sprite> normal,
               std::optional<Sprite> focused, std::optional<Sprite> highlight,
               glm::vec2 pos, RectF hoverBounds)
    : NormalSprite(normal),
      FocusedSprite(focused),
      HighlightSprite(highlight),
      HoverBounds(hoverBounds) {
  Enabled = true;
  Id = id;
  Bounds = RectF(pos.x, pos.y,
                 NormalSprite.has_value() ? NormalSprite->Bounds.Width : 0.0f,
                 NormalSprite.has_value() ? NormalSprite->Bounds.Height : 0.0f);
}

void Button::UpdateInput(float dt) {
  using namespace Impacto::Vm::Interface;

  if (!Enabled) return;
  const RectF& bounds = (HoverBounds != RectF{}) ? HoverBounds : Bounds;
  const bool useCursor = (Input::CurrentInputDevice == Input::Device::Mouse ||
                          Input::CurrentInputDevice == Input::Device::Touch);
  if (useCursor && Input::PrevMousePos != Input::CurMousePos) {
    Hovered = bounds.ContainsPoint(Input::CurMousePos);
  }

  const bool isClicked =
      GetControlState(ControlType::OK, InputDownType::WentDown);
  if (OnClickHandler && HasFocus && isClicked &&
      (!useCursor || (Hovered && bounds.ContainsPoint(Input::InitMousePos)))) {
    OnClickHandler(this);
  }
}

void Button::Render() {
  if (HasFocus && HighlightSprite.has_value()) {
    const RectF dest =
        HighlightSprite->ScaledBounds()
            .Scale({Bounds.Width / HighlightSprite->ScaledWidth(), 1.0f},
                   {0.0f, 0.0f})
            .Translate(Bounds.GetPos() + HighlightOffset);
    Renderer->DrawSprite(*HighlightSprite, dest, Tint);
  }

  if (IsLocked) {
    if (LockedSprite.has_value()) {
      Renderer->DrawSprite(*LockedSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
    }
  } else if (HasFocus && Enabled) {
    if (FocusedSprite.has_value()) {
      Renderer->DrawSprite(*FocusedSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
    }
  } else if (Enabled) {
    if (NormalSprite.has_value()) {
      Renderer->DrawSprite(*NormalSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
    }
  } else {
    if (DisabledSprite.has_value()) {
      Renderer->DrawSprite(*DisabledSprite, glm::vec2(Bounds.X, Bounds.Y),
                           Tint);
    }
  }

  if (HasText) {
    Profile::Dialogue::DialogueFont->DrawProcessedText(Text, Tint.a,
                                                       OutlineMode);
  }
}

void Button::SetText(Vm::BufferOffsetContext scrCtx, float fontSize,
                     RendererOutlineMode outlineMode,
                     DialogueColorPair colorPair) {
  HasText = true;
  Impacto::Vm::Sc3VmThread dummy;
  dummy.IpOffset = scrCtx.IpOffset;
  dummy.ScriptBufferId = scrCtx.ScriptBufferId;
  Text = TextLayoutPlainLine(
      &dummy, 255, *Profile::Dialogue::DialogueFont, fontSize, colorPair, 1.0f,
      glm::vec2(Bounds.X, Bounds.Y), TextAlignment::Left);
  OutlineMode = outlineMode;
  Bounds = GetTextBounds(Text);
  HoverBounds = Bounds;
}
void Button::SetText(Vm::BufferOffsetContext scrCtx, float fontSize,
                     RendererOutlineMode outlineMode, int colorIndex) {
  SetText(scrCtx, fontSize, outlineMode,
          Profile::Dialogue::ColorTable[colorIndex]);
}

void Button::SetText(Vm::Sc3Stream& stream, float fontSize,
                     RendererOutlineMode outlineMode,
                     DialogueColorPair colorPair) {
  HasText = true;
  Text = TextLayoutPlainLine(
      stream, 255, *Profile::Dialogue::DialogueFont, fontSize, colorPair, 1.0f,
      glm::vec2(Bounds.X, Bounds.Y), TextAlignment::Left);
  OutlineMode = outlineMode;
  Bounds = GetTextBounds(Text);
  HoverBounds = Bounds;
}

void Button::SetText(Vm::Sc3Stream& stream, float fontSize,
                     RendererOutlineMode outlineMode, int colorIndex) {
  SetText(stream, fontSize, outlineMode,
          Profile::Dialogue::ColorTable[colorIndex]);
}

void Button::SetText(std::vector<ProcessedTextGlyph> text, float textWidth,
                     float fontSize, RendererOutlineMode outlineMode) {
  HasText = true;
  Text = std::move(text);
  TextWidth = textWidth;
  OutlineMode = outlineMode;
  Bounds = GetTextBounds(Text);
  HoverBounds = Bounds;
}

void Button::Move(glm::vec2 relativePosition) {
  if (HasText) {
    for (ProcessedTextGlyph& glyph : Text) {
      glyph.Move(relativePosition);
    }
  }
  Widget::Move(relativePosition);
  if (HoverBounds != RectF{}) HoverBounds += relativePosition;
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto