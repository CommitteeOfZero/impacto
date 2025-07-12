#pragma once

#include <functional>

#include "../widget.h"
#include "../../text.h"
#include "../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace Widgets {

class Toggle : public Widget {
 public:
  Toggle(int id, bool* value, Sprite const& enabled, Sprite const& disabled,
         Sprite const& highlight, glm::vec2 pos, bool isCheckbox);
  Toggle(int id, bool* value, Sprite const& enabled, Sprite const& disabled,
         Sprite const& highlight, glm::vec2 pos, bool isCheckbox,
         Vm::Sc3Stream& stream, glm::vec2 labelOfs, float fontSize,
         RendererOutlineMode outlineMode);
  Toggle(int id, bool* value, Sprite const& enabled, Sprite const& disabled,
         Sprite const& highlight, glm::vec2 pos, bool isCheckbox,
         Sprite const& label, glm::vec2 labelOfs);

  void Update(float dt) override;
  void UpdateInput() override;
  void Render() override;

  int Id;
  bool* Value;
  std::function<void(Toggle*)> OnClickHandler;

 private:
  void SetText(Vm::Sc3Stream& stream, float fontSize,
               RendererOutlineMode outlineMode);

  Sprite EnabledSprite;
  Sprite DisabledSprite;
  Sprite HighlightSprite;

  bool IsCheckbox = false;

  bool HasSpriteLabel = false;
  Sprite LabelSprite;

  glm::vec2 LabelOffset;

  bool HasTextLabel = false;
  std::vector<ProcessedTextGlyph> Label;
  float FontSize;
  float TextWidth = 0.0f;
  RendererOutlineMode OutlineMode;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto