#include "moviemenuentrybutton.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/games/chlcc/moviemenu.h"
#include "../../../profile/scriptvars.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::MovieMenu;
using namespace Impacto::Profile::ScriptVars;

MovieMenuEntryButton::MovieMenuEntryButton(int id, Sprite const& movieThumbnail,
                                           Sprite const& lockedMovieThumbnail,
                                           glm::vec2 thumbnailPos,
                                           glm::vec2 boxPos) {
  Id = id;
  NormalSprite = movieThumbnail;
  DisabledSprite = lockedMovieThumbnail;
  Enabled = true;
  Bounds = RectF(boxPos.x, boxPos.y, MovieBox.ScaledWidth(),
                 MovieBox.ScaledHeight());

  MovieBoxAnim = SelectedMovieAnimation.Instantiate();
  MovieBoxAnim.Direction = AnimationDirection::In;
  MovieBoxAnim.LoopMode = AnimationLoopMode::Loop;
  MovieBoxAnim.StartIn();
}

void MovieMenuEntryButton::Update(float dt) {
  Button::Update(dt);
  MovieBoxAnim.Update(dt);
}

void MovieMenuEntryButton::Render() {
  if (HasFocus) {
    Renderer->DrawSprite(MovieBoxAnim.CurrentSprite(), Bounds);
    Renderer->DrawSprite(SelectedMovieYellowDot,
                         glm::vec2(Bounds.X - 7, Bounds.Y + 53));
  } else {
    Renderer->DrawSprite(MovieBox, Bounds);
  }
  if (!IsLocked) {
    Renderer->DrawSprite(NormalSprite, glm::vec2(Bounds.X + 20, Bounds.Y + 16),
                         Tint);
  } else {
    Renderer->DrawSprite(DisabledSprite,
                         glm::vec2(Bounds.X + 20, Bounds.Y + 16), Tint);
  }
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto