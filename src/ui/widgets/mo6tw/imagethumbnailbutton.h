#pragma once

#include "../button.h"
#include "../label.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

class ImageThumbnailButton : public Widgets::Button {
 public:
  ImageThumbnailButton(int id, Sprite const& norm, Sprite const& disabled,
                       Sprite const& focusedTopLeft,
                       Sprite const& focusedTopRight,
                       Sprite const& focusedBottomLeft,
                       Sprite const& focusedBottomRight, glm::vec2 pos);
  ImageThumbnailButton(int id, Sprite const& normTopPart,
                       Sprite const& normBottomPart, Sprite const& disabled,
                       Sprite const& focusedTopLeft,
                       Sprite const& focusedTopRight,
                       Sprite const& focusedBottomLeft,
                       Sprite const& focusedBottomRight, glm::vec2 pos);
  void Update(float dt) override;
  void Render() override;

  bool IsLocked = true;

 private:
  bool IsSplit = false;
  Sprite BottomPart;
  Sprite HighlightTopLeft;
  Sprite HighlightTopRight;
  Sprite HighlightBottomLeft;
  Sprite HighlightBottomRight;

  Animation HighlightAnimation;
};

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto