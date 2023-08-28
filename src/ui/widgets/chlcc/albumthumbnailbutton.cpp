#include "albumthumbnailbutton.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

glm::vec4 AlbumThumbnailButton::FocusedAlpha = glm::vec4(1.0f);
Animation AlbumThumbnailButton::FocusedAlphaFade;

void AlbumThumbnailButton::Render() {
  Renderer->DrawSprite((IsLocked ? LockedSprite : NormalSprite),
                       glm::vec2(Bounds.X, Bounds.Y));
  if (!IsLocked) {
    for (int variations = 0; variations < totalVariations; variations++) {
      Renderer->DrawSprite(
          (viewedVariations - variations > 0 ? VariationUnlocked
                                             : VariationLocked),
          glm::vec2(Bounds.X + VariationTemplateOffset.x -
                        VariationUnlocked.Bounds.Width * variations,
                    Bounds.Y + VariationTemplateOffset.y));
    }
  }
  if (HasFocus) {
    Renderer->DrawSprite(HighlightSprite, glm::vec2(Bounds.X, Bounds.Y),
                         FocusedAlpha);
  }
}

void AlbumThumbnailButton::FocusedAlphaFadeStart() {
  if (FocusedAlphaFade.State == AS_Stopped) {
    FocusedAlphaFade.Direction = 1;
    FocusedAlphaFade.DurationIn = 0.5f;
    FocusedAlphaFade.DurationOut = 0.5f;
    FocusedAlphaFade.LoopMode = ALM_ReverseDirection;
    FocusedAlphaFade.StartIn();
  }
}

void AlbumThumbnailButton::FocusedAlphaFadeReset() {
  FocusedAlphaFade.State = AS_Stopped;
  FocusedAlphaFade.Progress = 0.0f;
}

void AlbumThumbnailButton::UpdateFocusedAlphaFade(float dt) {
  FocusedAlphaFade.Update(dt);
  FocusedAlpha =
      glm::vec4(glm::vec3(1.0f), ((FocusedAlphaFade.Progress * 30) + 1) / 85);
}

void AlbumThumbnailButton::UpdateVariations(int total, int viewed) {
  totalVariations = total;
  viewedVariations = viewed;
  IsLocked = (viewedVariations < totalVariations);
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
