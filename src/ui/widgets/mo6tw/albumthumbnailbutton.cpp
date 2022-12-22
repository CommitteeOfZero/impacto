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
  Vm::Sc3VmThread dummy;
  char temp[10];
  uint16_t sc3StringBuffer[10];

  if (unlockedVariations > 0)
    sprintf(temp, "%d/%d", unlockedVariations, totalVariations);
  else
    sprintf(temp, "??/??");
  TextGetSc3String(std::string(temp), sc3StringBuffer);
  dummy.Ip = (uint8_t*)sc3StringBuffer;
  InfoTextWidth = TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont,
                                        ThumbnailButtonTextFontSize);
  InfoText = new Label(
      (uint8_t*)sc3StringBuffer,
      pos +
          glm::vec2(norm.Bounds.Width - InfoTextWidth,
                    norm.Bounds.Height - ThumbnailButtonTextFontSize) +
          ThumbnailButtonTextOffset,
      ThumbnailButtonTextFontSize, true, ThumbnailButtonTextColorIndex);
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

void AlbumThumbnailButton::MoveTo(glm::vec2 pos) {
  InfoText->MoveTo(pos);
  Widget::MoveTo(pos);
}

}  // namespace MO6TW
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto