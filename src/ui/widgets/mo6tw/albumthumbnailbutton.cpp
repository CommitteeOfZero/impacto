#include "albumthumbnailbutton.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/games/mo6tw/albummenu.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace MO6TW {

using namespace Impacto::Profile::MO6TW::AlbumMenu;

AlbumThumbnailButton::AlbumThumbnailButton(
    int id, Sprite const& norm, Sprite const& disabled,
    Sprite const& focusedTopLeft, Sprite const& focusedTopRight,
    Sprite const& focusedBottomLeft, Sprite const& focusedBottomRight,
    Sprite const& border, int unlockedVariations, int totalVariations,
    glm::vec2 pos)
    : ImageThumbnailButton(id, norm, disabled, focusedTopLeft, focusedTopRight,
                           focusedBottomLeft, focusedBottomRight, pos) {
  TotalVariations = totalVariations;
  UnlockedVariations = unlockedVariations;
  uint16_t sc3StringBuffer[10];

  std::string variationsCount =
      (unlockedVariations > 0)
          ? fmt::format("{:d}/{:d}", unlockedVariations, totalVariations)
          : "\?\?/\?\?";
  TextGetSc3String(variationsCount, sc3StringBuffer);
  Vm::Sc3Stream stream(sc3StringBuffer);

  InfoTextWidth = TextGetPlainLineWidth(stream, Profile::Dialogue::DialogueFont,
                                        ThumbnailButtonTextFontSize);
  stream = Vm::Sc3Stream(sc3StringBuffer);
  InfoText = new Label(
      stream,
      pos +
          glm::vec2(norm.Bounds.Width - InfoTextWidth,
                    norm.Bounds.Height - ThumbnailButtonTextFontSize) +
          ThumbnailButtonTextOffset,
      ThumbnailButtonTextFontSize, RendererOutlineMode::Full,
      ThumbnailButtonTextColorIndex);
  Border = border;
}

void AlbumThumbnailButton::Render() {
  Renderer->DrawSprite(
      Border, glm::vec2(Bounds.X, Bounds.Y) + ThumbnailButtonBorderOffset,
      Tint);
  ImageThumbnailButton::Render();
  InfoText->Render();
}

void AlbumThumbnailButton::Move(glm::vec2 relativePosition) {
  InfoText->Move(relativePosition);
  Widget::Move(relativePosition);
}

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto