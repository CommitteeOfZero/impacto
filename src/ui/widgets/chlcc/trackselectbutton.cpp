#include "trackselectbutton.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

TrackSelectButton::TrackSelectButton(int id, Sprite const &focused,
                                     glm::vec2 pos, glm::vec2 numOffset,
                                     glm::vec2 trackOffset,
                                     glm::vec2 artistOffset)
    : Button(id, Sprite(), focused, Sprite(), pos),
      TrackNumPos(pos + numOffset),
      TrackTextPos(pos + trackOffset),
      ArtistTextPos(pos + artistOffset) {
  const std::string label = fmt::sprintf("%02d", id + 1);
  TrackNum = Label(label, TrackNumPos, 20, RendererOutlineMode::None, 0);
  HasText = true;
  Bounds = RectF(pos.x, pos.y, FocusedSprite.Bounds.Width - pos.x,
                 FocusedSprite.Bounds.Height);
}

void TrackSelectButton::SetTrackText(Vm::BufferOffsetContext strAdr) {
  TrackName = Label(strAdr, TrackTextPos, 20, RendererOutlineMode::None, 0);
}

void TrackSelectButton::SetArtistText(Vm::BufferOffsetContext strAdr) {
  Artist = Label(strAdr, ArtistTextPos, 20, RendererOutlineMode::None, 0);
}

void TrackSelectButton::Render() {
  if (HasFocus) {
    Renderer->DrawSprite(FocusedSprite, glm::vec2(0, Bounds.Y));
  }

  TrackNum.Render();
  TrackName.Render();
  Artist.Render();
}

void TrackSelectButton::MoveTracks(glm::vec2 baseline) {
  TrackNum.MoveTo(TrackNumPos + baseline);
  TrackName.MoveTo(TrackTextPos + baseline);
  Artist.MoveTo(ArtistTextPos + baseline);
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
