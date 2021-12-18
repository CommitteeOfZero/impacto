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
                       glm::vec2 pos);
  void Render() override;

 private:
  Sprite Border;
};

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto