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

Button::Button(int id, Sprite const& norm, Sprite const& focused,
               Sprite const& highlight, glm::vec2 pos) {
  Enabled = true;
  Id = id;
  NormalSprite = norm;
  FocusedSprite = focused;
  HighlightSprite = highlight;
  Bounds = RectF(pos.x, pos.y, NormalSprite.Bounds.Width,
                 NormalSprite.Bounds.Height);
}

void Button::UpdateInput() {
  if (Enabled) {
    if (Input::CurrentInputDevice == Input::InputDevice::IDEV_Mouse &&
        Input::PrevMousePos != Input::CurMousePos) {
      Hovered = Bounds.ContainsPoint(Input::CurMousePos);
    } else if (Input::CurrentInputDevice == Input::InputDevice::IDEV_Touch &&
               Input::TouchIsDown[0] &&
               Input::PrevTouchPos != Input::CurTouchPos) {
      Hovered = Bounds.ContainsPoint(Input::CurTouchPos);
    }
    if (OnClickHandler && HasFocus &&
        ((Hovered &&
          Vm::Interface::PADinputMouseWentDown & Vm::Interface::PAD1A) ||
         (Vm::Interface::PADinputButtonWentDown & Vm::Interface::PAD1A))) {
      OnClickHandler(this);
    }
  }
}

void Button::Render() {
  if (HasFocus) {
    Renderer->DrawSprite(
        HighlightSprite,
        glm::vec2(Bounds.X + HighlightOffset.x, Bounds.Y + HighlightOffset.y),
        Tint, glm::vec2(Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f));
  }

  if (IsLocked) {
    Renderer->DrawSprite(LockedSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  } else if (HasFocus && Enabled) {
    Renderer->DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  } else if (Enabled) {
    Renderer->DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  } else {
    Renderer->DrawSprite(DisabledSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  }

  if (HasText) {
    Renderer->DrawProcessedText(Text, Profile::Dialogue::DialogueFont, Tint.a,
                                OutlineMode, true);
  }
}

void Button::SetText(uint8_t* str, float fontSize,
                     RendererOutlineMode outlineMode, int colorIndex) {
  HasText = true;
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  Text = TextLayoutPlainLine(
      &dummy, 255, Profile::Dialogue::DialogueFont, fontSize,
      Profile::Dialogue::ColorTable[colorIndex], 1.0f,
      glm::vec2(Bounds.X, Bounds.Y), TextAlignment::Left);
  OutlineMode = outlineMode;
  for (int i = 0; i < Text.size(); i++) {
    TextWidth += Text[i].DestRect.Width;
  }
  Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize);
}

void Button::SetText(std::vector<ProcessedTextGlyph> text, float textWidth,
                     float fontSize, RendererOutlineMode outlineMode) {
  HasText = true;
  Text = std::move(text);
  TextWidth = textWidth;
  OutlineMode = outlineMode;
  Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize);
}

void Button::Move(glm::vec2 relativePosition) {
  if (HasText) {
    for (int i = 0; i < Text.size(); i++) {
      Text[i].DestRect.X += relativePosition.x;
      Text[i].DestRect.Y += relativePosition.y;
    }
  }
  Widget::Move(relativePosition);
}

void Button::Move(glm::vec2 relativePosition, float duration) {
  Widget::Move(relativePosition, duration);
}

void Button::MoveTo(glm::vec2 pos) {
  auto relativePosition = pos - glm::vec2(Bounds.X, Bounds.Y);
  Move(relativePosition);
}

void Button::MoveTo(glm::vec2 pos, float duration) {
  Widget::MoveTo(pos, duration);
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto