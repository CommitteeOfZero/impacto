#include "albumthumbnailbutton.h"

#include "../../../renderer2d.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

AlbumThumbnailButton::AlbumThumbnailButton(int id, Sprite const& norm,
                                           Sprite const& disabled,
                                           Sprite const& focusedTopLeft,
                                           Sprite const& focusedTopRight,
                                           Sprite const& focusedBottomLeft,
                                           Sprite const& focusedBottomRight,
                                           Sprite const& border, glm::vec2 pos)
    : ImageThumbnailButton(id, norm, disabled, focusedTopLeft, focusedTopRight,
                           focusedBottomLeft, focusedBottomRight, pos) {
  Border = border;
}

void AlbumThumbnailButton::Render() {
  Renderer2D::DrawSprite(Border, glm::vec2(Bounds.X - 8.0f, Bounds.Y - 9.0f),
                         Tint);
  ImageThumbnailButton::Render();
}

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto