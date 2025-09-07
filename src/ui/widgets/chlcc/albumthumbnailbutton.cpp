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
    for (int variations = 0; variations < TotalVariations; variations++) {
      Renderer->DrawSprite(
          (ViewedVariations > variations ? VariationUnlocked : VariationLocked),
          glm::vec2(Bounds.X + VariationTemplateOffset.x -
                        VariationUnlocked.Bounds.Width * (float)variations,
                    Bounds.Y + VariationTemplateOffset.y));
    }
  }
  if (HasFocus) {
    Renderer->DrawSprite(SelectionMarker,
                         glm::vec2(Bounds.X, Bounds.Y) + SelectionMarkerOffset);
    Renderer->DrawSprite(HighlightSprite, glm::vec2(Bounds.X, Bounds.Y),
                         FocusedAlpha);
  }
}

void AlbumThumbnailButton::FocusedAlphaFadeStart() {
  if (FocusedAlphaFade.State == AnimationState::Stopped) {
    FocusedAlphaFade.Direction = AnimationDirection::In;
    FocusedAlphaFade.SetDuration(0.5f);
    FocusedAlphaFade.LoopMode = AnimationLoopMode::ReverseDirection;
    FocusedAlphaFade.StartIn();
  }
}

void AlbumThumbnailButton::FocusedAlphaFadeReset() {
  FocusedAlphaFade.State = AnimationState::Stopped;
  FocusedAlphaFade.Progress = 0.0f;
}

void AlbumThumbnailButton::UpdateFocusedAlphaFade(float dt) {
  FocusedAlphaFade.Update(dt);
  FocusedAlpha =
      glm::vec4(glm::vec3(1.0f), ((FocusedAlphaFade.Progress * 30) + 1) / 85);
}

void AlbumThumbnailButton::UpdateVariations(int total, int viewed) {
  TotalVariations = total;
  ViewedVariations = viewed;
  IsLocked = (ViewedVariations < TotalVariations);
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
