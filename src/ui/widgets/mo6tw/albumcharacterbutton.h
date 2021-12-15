#pragma once

#include "../button.h"
#include "../label.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

class AlbumCharacterButton : public Widgets::Button {
 public:
  AlbumCharacterButton(int id, Sprite const& norm, Sprite const& locked,
                       Sprite const& highlight, Sprite const& lockedHighlight,
                       glm::vec2 pos, float highlightAnimationDuration);
  void Update(float dt) override;
  void Render() override;

 private:
  Sprite LockedHighlightSprite;
  Animation HighlightAnimation;
};

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto