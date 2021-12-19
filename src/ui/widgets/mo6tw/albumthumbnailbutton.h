#pragma once

#include "imagethumbnailbutton.h"
#include "../label.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

class AlbumThumbnailButton : public ImageThumbnailButton {
 public:
  AlbumThumbnailButton(int id, Sprite const& norm, Sprite const& disabled,
                       Sprite const& focusedTopLeft,
                       Sprite const& focusedTopRight,
                       Sprite const& focusedBottomLeft,
                       Sprite const& focusedBottomRight, Sprite const& border,
                       int unlockedVariations, int totalVariations,
                       glm::vec2 pos);
  void Render() override;
  void Move(glm::vec2 relativePosition) override;
  void MoveTo(glm::vec2 pos) override;

 private:
  Sprite Border;
  int UnlockedVariations = 0;
  int TotalVariations = 0;
  Widgets::Label* InfoText;
  float InfoTextWidth;
};

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto