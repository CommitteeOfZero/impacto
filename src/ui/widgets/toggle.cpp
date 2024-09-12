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
               bool isCheckbox, uint8_t* str, glm::vec2 labelOfs, float fontSize,
               RendererOutlineMode outlineMode)
    : Toggle(id, value, enabled, disabled, highlight, pos, isCheckbox) {
  HasTextLabel = true;
  SetText(str, fontSize, outlineMode);
}

Toggle::Toggle(int id, bool* value, Sprite const& enabled,
               Sprite const& disabled, Sprite const& highlight, glm::vec2 pos,
               bool isCheckbox, Sprite const& label, glm::vec2 labelOfs)
    : Toggle(id, value, enabled, disabled, highlight, pos, isCheckbox) {
  HasSpriteLabel = true;
  LabelSprite = label;
  LabelOffset = labelOfs;
}

void Toggle::UpdateInput() {
  if (Enabled) {
    if (Input::PrevMousePos != Input::CurMousePos) {
      Hovered = Bounds.ContainsPoint(Input::CurMousePos);
    }
    if (HasFocus &&
        ((Hovered &&
          Vm::Interface::PADinputMouseWentDown & Vm::Interface::PAD1A) ||
         (Vm::Interface::PADinputButtonWentDown & Vm::Interface::PAD1A))) {
      *Value = !*Value;
      if (OnClickHandler) OnClickHandler(this);
    }
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
    Renderer->DrawProcessedText(Label, Profile::Dialogue::DialogueFont, Tint.a,
                                OutlineMode, true);
  }
}

void Toggle::SetText(uint8_t* str, float fontSize,
                     RendererOutlineMode outlineMode) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  Label = TextLayoutPlainLine(&dummy, 255, Profile::Dialogue::DialogueFont,
                              fontSize, Profile::Dialogue::ColorTable[10], 1.0f,
                              glm::vec2(Bounds.X, Bounds.Y) + LabelOffset,
                              TextAlignment::Left);
  OutlineMode = outlineMode;
  FontSize = fontSize;
  for (int i = 0; i < Label.size(); i++) {
    TextWidth += Label[i].DestRect.Width;
  }
}

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto