#pragma once

#include <functional>

#include "../widget.h"
#include "../../text.h"

namespace Impacto {
namespace UI {
namespace Widgets {

class Toggle : public Widget {
 public:
  Toggle(int id, bool* value, Sprite const& enabled, Sprite const& disabled,
         Sprite const& highlight, glm::vec2 pos, bool isCheckbox);
  Toggle(int id, bool* value, Sprite const& enabled, Sprite const& disabled,
         Sprite const& highlight, glm::vec2 pos, bool isCheckbox, uint8_t* str,
         glm::vec2 labelOfs, int fontSize, bool outline);
  Toggle(int id, bool* value, Sprite const& enabled, Sprite const& disabled,
         Sprite const& highlight, glm::vec2 pos, bool isCheckbox,
         Sprite const& label, glm::vec2 labelOfs);

  void Update(float dt) override;
  void UpdateInput();
  void Render();

  int Id;
  std::function<void(Toggle*)> OnClickHandler;

 private:
  void SetText(uint8_t* str, int fontSize, bool outline);

  bool* Value;
  Sprite EnabledSprite;
  Sprite DisabledSprite;
  Sprite HighlightSprite;

  bool IsCheckbox = false;

  bool HasSpriteLabel = false;
  Sprite LabelSprite;

  glm::vec2 LabelOffset;

  bool HasTextLabel = false;
  ProcessedTextGlyph Label[255];
  int FontSize;
  int TextLength;
  float TextWidth = 0.0f;
  bool Outline = false;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto