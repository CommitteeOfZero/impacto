#pragma once

#include "../button.h"
#include "../label.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

class ActorsVoiceButton : public Widgets::Button {
 public:
  ActorsVoiceButton(int id, Sprite const& norm, Sprite const& locked,
                    Sprite const& highlight, Sprite const& lockedHighlight,
                    glm::vec2 pos);
  void Render() override;

  bool IsLocked = true;

 private:
  Sprite LockedHighlightSprite;
};

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto