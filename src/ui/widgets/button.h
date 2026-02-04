#pragma once

#include <functional>

#include "../widget.h"
#include "../../spritesheet.h"
#include "../../text/text.h"
#include "../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace Widgets {

class Button : public Widget {
 public:
  Button() {}
  Button(int id, Sprite const& norm, Sprite const& focused,
         Sprite const& highlight, glm::vec2 pos, RectF hoverBounds = RectF{});

  virtual void UpdateInput(float dt) override;
  virtual void Render() override;

  using Widget::Move;
  virtual void Move(glm::vec2 relativePosition) override;

  void SetText(Vm::BufferOffsetContext scrCtx, float fontSize,
               RendererOutlineMode outlineMode, int colorIndex = 10);
  void SetText(Vm::BufferOffsetContext scrCtx, float fontSize,
               RendererOutlineMode outlineMode, DialogueColorPair colorPair);
  void SetText(Vm::Sc3Stream& stream, float fontSize,
               RendererOutlineMode outlineMode, int colorIndex = 10);
  void SetText(Vm::Sc3Stream& stream, float fontSize,
               RendererOutlineMode outlineMode, DialogueColorPair colorPair);
  virtual void SetText(std::vector<ProcessedTextGlyph> text, float textWidth,
                       float fontSize, RendererOutlineMode outlineMode);
  void ClearText() {
    Text.clear();
    Bounds = {};
    HoverBounds = {};
    HasText = false;
  }

  std::function<void(Button*)> OnClickHandler;

  Sprite NormalSprite;
  Sprite FocusedSprite;
  Sprite HighlightSprite;
  Sprite DisabledSprite;
  Sprite LockedSprite;
  RectF HoverBounds;
  glm::vec2 HighlightOffset = glm::vec2(0.0f, 3.0f);

  int Id;
  bool IsLocked = false;

 protected:
  bool HasText = false;
  std::vector<ProcessedTextGlyph> Text;
  float TextWidth = 0.0f;
  RendererOutlineMode OutlineMode;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto