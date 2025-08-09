#pragma once

#include "toggle.h"
#include "scrollbar.h"
#include "../widget.h"
#include "../menu.h"
#include "../../text.h"
#include "../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace Widgets {

class OptionGroup : public Widget {
 public:
  OptionGroup(Menu* menuContext, Sprite const& background,
              Sprite const& highlight, Sprite const& itemHighlight,
              glm::vec2 pos, glm::vec2 itemsOffset);

  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;

  float ItemPadding = 0.0f;

  void AddOption(Toggle* item);
  void AddOption(Scrollbar* item);

 private:
  bool GroupEntered = false;
  int LastFocusableElementId = -1;

  Menu* MenuContext;
  glm::vec2 Position;
  glm::vec2 CurrentItemPosition;
  Sprite BackgroundSprite;
  Sprite BackgroundHSprite;
  Sprite ItemHighlightSprite;
  std::vector<Widget*> Items;
};

}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto