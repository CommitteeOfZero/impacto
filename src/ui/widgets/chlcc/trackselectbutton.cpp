#include "trackselectbutton.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

TrackSelectButton::TrackSelectButton(int id, Sprite const &focused,
                                     glm::vec2 pos, glm::vec2 trackOffset,
                                     glm::vec2 artistOffset)
    : Button(id, Sprite(), focused, Sprite(), pos),
      TrackTextPos(pos + trackOffset),
      ArtistTextPos(pos + artistOffset) {
  HasText = true;
  Bounds = RectF(pos.x, pos.y, FocusedSprite.Bounds.Width - pos.x,
                 FocusedSprite.Bounds.Height);
}

void TrackSelectButton::SetTrackText(uint8_t *str) {
  TrackName = Label(str, TrackTextPos, 20, RO_None, 0);
}

void TrackSelectButton::SetArtistText(uint8_t *str) {
  Artist = Label(str, ArtistTextPos, 20, RO_None, 0);
}

void TrackSelectButton::Render() {
  if (HasFocus) {
    Renderer->DrawSprite(FocusedSprite, glm::vec2(0, Bounds.Y));
  }
  TrackName.Render();
  Artist.Render();
}

void TrackSelectButton::MoveTracks(glm::vec2 baseline) {
  TrackName.MoveTo(TrackTextPos + baseline);
  Artist.MoveTo(ArtistTextPos + baseline);
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
