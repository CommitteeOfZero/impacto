#include "button.h"
#include "../../renderer2d.h"
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
    if (Input::PrevMousePos != Input::CurMousePos) {
      Hovered = Bounds.ContainsPoint(Input::CurMousePos);
    }
    if (HasFocus &&
        ((Hovered &&
          Vm::Interface::PADinputMouseWentDown & Vm::Interface::PAD1A) ||
         (Vm::Interface::PADinputButtonWentDown & Vm::Interface::PAD1A))) {
      OnClickHandler(this);
    }
  }
}

void Button::Render() {
  if (HasFocus) {
    Renderer2D::DrawSprite(
        HighlightSprite,
        glm::vec2(Bounds.X + HighlightOffset.x, Bounds.Y + HighlightOffset.y),
        Tint, glm::vec2(Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f));
    Renderer2D::DrawSprite(FocusedSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
  } else {
    if (Enabled)
      Renderer2D::DrawSprite(NormalSprite, glm::vec2(Bounds.X, Bounds.Y), Tint);
    else
      Renderer2D::DrawSprite(DisabledSprite, glm::vec2(Bounds.X, Bounds.Y),
                             Tint);
  }

  if (HasText) {
    Renderer2D::DrawProcessedText(Text, TextLength,
                                  Profile::Dialogue::DialogueFont, Tint.a,
                                  Outline, true);
  }
}

void Button::SetText(uint8_t* str, int fontSize, bool outline, int colorIndex) {
  HasText = true;
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  TextLength = TextLayoutPlainLine(
      &dummy, 255, Text, Profile::Dialogue::DialogueFont, fontSize,
      Profile::Dialogue::ColorTable[colorIndex], 1.0f,
      glm::vec2(Bounds.X, Bounds.Y), TextAlignment::Left);
  Outline = outline;
  for (int i = 0; i < TextLength; i++) {
    TextWidth += Text[i].DestRect.Width;
  }
  Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize);
}

void Button::SetText(ProcessedTextGlyph* str, int textLength, float textWidth,
                     int fontSize, bool outline) {
  HasText = true;
  TextLength = textLength;
  TextWidth = textWidth;
  Outline = outline;
  memcpy(Text, str, TextLength * sizeof(ProcessedTextGlyph));
  Bounds = RectF(Text[0].DestRect.X, Text[0].DestRect.Y, TextWidth, fontSize);
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto