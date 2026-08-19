#pragma once

#include <span>
#include "../../vm/vm.h"
#include "../../vm/sc3stream.h"
#include "../widget.h"
#include "../../text/text.h"
#include "../../renderer/renderer.h"
#include "../../profile/dialogue.h"

namespace Impacto {
namespace UI {
namespace Widgets {

using namespace Impacto::Fonts;

template <typename T>
concept LabelStringType =
    is_any_of_v<std::decay_t<T>, Vm::BufferOffsetContext, Vm::Sc3Stream> ||
    std::is_convertible_v<T, const std::string_view>;

class Label final : public Widget {
 public:
  Label();

  Label(Sprite const& label, glm::vec2 pos);

  Label(std::vector<ProcessedTextGlyph>&& str,
        RendererOutlineMode outlineMode) {
    SetText(std::move(str), outlineMode);
  }
  Label(std::span<const ProcessedTextGlyph> str,
        RendererOutlineMode outlineMode)
      : Label(std::vector<ProcessedTextGlyph>(str.begin(), str.end()),
              outlineMode) {}

  Label(LabelStringType auto str, glm::vec2 pos, float fontSize,
        RendererOutlineMode outlineMode, DialogueColorPair colorPair)
      : FontSize(fontSize) {
    Bounds = RectF(pos.x, pos.y, 0.0f, FontSize);
    SetText(str, fontSize, outlineMode, colorPair);
  }

  Label(LabelStringType auto str, glm::vec2 pos, float fontSize,
        RendererOutlineMode outlineMode, size_t colorIndex)
      : Label(str, pos, fontSize, outlineMode,
              Profile::Dialogue::ColorTable[colorIndex]) {}

  void UpdateInput(float dt) override {}
  void Render() override;

  using Widget::Move;
  void Move(glm::vec2 relativePosition) override;

  void SetSprite(Sprite const& label);

  void SetText(std::vector<ProcessedTextGlyph>&& str,
               RendererOutlineMode outlineMode);
  void SetText(std::span<const ProcessedTextGlyph> str,
               RendererOutlineMode outlineMode) {
    SetText(std::vector<ProcessedTextGlyph>(str.begin(), str.end()),
            outlineMode);
  }

  void SetText(LabelStringType auto str, float fontSize,
               RendererOutlineMode outlineMode, DialogueColorPair colorPair);

  void SetText(LabelStringType auto str, float fontSize,
               RendererOutlineMode outlineMode, size_t colorIndex = 10) {
    SetText(str, pos, fontSize, outlineMode,
            Profile::Dialogue::ColorTable[colorIndex]);
  }

  void ClearText() {
    Text.clear();
    IsText = false;
    Bounds = {};
  }

  size_t GetTextLength() const { return Text.size(); }
  float GetFontSize() const { return FontSize; }

  float OutlineAlpha = 1.0f;
  bool OutlineAlphaEnabled = false;

 protected:
  bool IsText;
  Sprite LabelSprite;
  float FontSize;
  std::vector<ProcessedTextGlyph> Text;
  float TextWidth = 0.0f;
  RendererOutlineMode OutlineMode = RendererOutlineMode::None;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto